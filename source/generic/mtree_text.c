/*
 * contrib/mtree_gist/mtree_text.c
 */
#include "postgres.h"

#include "mtree_gist.h"
#include "mtree_utils_var.h"
#include "utils/builtins.h"

#define SameStrategyNumber      1
#define OverlapStrategyNumber   2
#define ContainsStrategyNumber  3
#define ContainedStrategyNumber 4
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define OWNHDRSZ (3 * sizeof(int))
#define DatumGetMtreeGist(x) ((GMT_VARKEY*) PG_DETOAST_DATUM(x))
#define PG_GETARG_GMT_P(x) DatumGetMtreeGist(PG_GETARG_DATUM(x))
#define PG_RETURN_GMT_P(x) PG_RETURN_POINTER(x)

typedef enum {
  First,
  Best
} UnionStrategy;

typedef enum {
  Random,
  FirstTwo,
  MaxDistanceFromFirst,
  MaxDistancePair,
  SamplingMinCoveringSum,
  SamplingMinCoveringMax,
  SamplingMinOverlapArea,
  SamplingMinAreaSum
} PicksplitStrategy;

UnionStrategy unionStrategy = Best;
PicksplitStrategy picksplitStrategy = Random;

PG_FUNCTION_INFO_V1(gmt_text_compress);
PG_FUNCTION_INFO_V1(gmt_bpchar_compress);
PG_FUNCTION_INFO_V1(gmt_text_union);
PG_FUNCTION_INFO_V1(gmt_text_picksplit);
PG_FUNCTION_INFO_V1(gmt_text_consistent);
PG_FUNCTION_INFO_V1(gmt_bpchar_consistent);
PG_FUNCTION_INFO_V1(gmt_text_penalty);
PG_FUNCTION_INFO_V1(gmt_text_same);

/**************************************************
 * For comparison
 **************************************************/

static bool gmt_textgt(const void* a, const void* b, Oid collation, FmgrInfo* flinfo)
{
  return DatumGetBool(DirectFunctionCall2Coll(text_gt,
                                              collation,
                                              PointerGetDatum(a),
                                              PointerGetDatum(b)));
}

static bool gmt_textge(const void* a, const void* b, Oid collation, FmgrInfo* flinfo)
{
  return DatumGetBool(DirectFunctionCall2Coll(text_ge,
                                              collation,
                                              PointerGetDatum(a),
                                              PointerGetDatum(b)));
}

static bool gmt_texteq(const void* a, const void* b, Oid collation, FmgrInfo* flinfo)
{
  return DatumGetBool(DirectFunctionCall2Coll(texteq,
                                              collation,
                                              PointerGetDatum(a),
                                              PointerGetDatum(b)));
}

static bool gmt_textle(const void* a, const void* b, Oid collation, FmgrInfo* flinfo)
{
  return DatumGetBool(DirectFunctionCall2Coll(text_le,
                                              collation,
                                              PointerGetDatum(a),
                                              PointerGetDatum(b)));
}

static bool gmt_textlt(const void* a, const void* b, Oid collation, FmgrInfo* flinfo)
{
  return DatumGetBool(DirectFunctionCall2Coll(text_lt,
                                              collation,
                                              PointerGetDatum(a),
                                              PointerGetDatum(b)));
}

static int32 gmt_textcmp(const void* a, const void* b, Oid collation, FmgrInfo* flinfo)
{
  return DatumGetInt32(DirectFunctionCall2Coll(bttextcmp,
                                               collation,
                                               PointerGetDatum(a),
                                               PointerGetDatum(b)));
}

/* Type info */
static gmtree_vinfo tinfo =
{
  gmt_t_text,
  0,
  false,
  gmt_textgt,
  gmt_textge,
  gmt_texteq,
  gmt_textle,
  gmt_textlt,
  gmt_textcmp,
  NULL
};

/**************************************************
 * Useful functions
 **************************************************/

int string_distance(const char* a, const char* b)
{
  int lengthOfA = (int) strlen(a);
  int lengthOfB = (int) strlen(b);
  int x, y, lastDiagonal, oldDiagonal;
  int column[lengthOfA + 1];

  column[0] = 0;
  for (y = 1; y <= lengthOfA; ++y)
  {
    column[y] = y;
  }

  for (x = 1; x <= lengthOfB; ++x)
  {
    column[0] = x;
    for (y = 1, lastDiagonal = x - 1; y <= lengthOfA; ++y)
    {
      oldDiagonal = column[y];
      column[y] = MIN3(
        column[y] + 1,
        column[y - 1] + 1,
        lastDiagonal + (a[y - 1] == b[x - 1] ? 0 : 1)
      );
      lastDiagonal = oldDiagonal;
    }
  }

  return column[lengthOfA];
}

bool text_equals(GMT_VARKEY* first, GMT_VARKEY* second)
{
  return !strcmp(first->vl_dat, second->vl_dat);
}

bool text_overlap(GMT_VARKEY* first, GMT_VARKEY* second, int distance)
{
  return distance <= first->coveringRadius + second->coveringRadius;
}

bool text_contains(GMT_VARKEY* big, GMT_VARKEY* small, int distance)
{
  return big->coveringRadius >= distance + small->coveringRadius;
}

GMT_VARKEY* copy_text(GMT_VARKEY *source)
{
  GMT_VARKEY* destination = (GMT_VARKEY*) palloc(VARSIZE_ANY(source));

  memcpy(destination, source, VARSIZE_ANY(source));

  return destination;
}

void init_matrix(int size, int distances[size][size])
{
  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      distances[i][j] = -1;
    }
  }
}

int get_matrix_distance(int size, GMT_VARKEY *entries[size], int distances[size][size], int i, int j)
{
  if (distances[i][j] == -1)
  {
    distances[i][j] = string_distance(entries[i]->vl_dat, entries[j]->vl_dat);
  }
  return distances[i][j];
}

double get_area_overlap(int radiusOne, int radiusTwo, int distance)
{
  if (radiusOne == 0 || radiusTwo == 0 || distance == 0)
  {
    return 0;
  }

  int radiusOneSquare = radiusOne * radiusOne;
  int radiusTwoSquare = radiusTwo * radiusTwo;
  int distanceSquare = distance * distance;

  double phi = (acos((radiusOneSquare + distanceSquare - radiusTwoSquare) / (2 * radiusOne * distance))) * 2;
  double theta = (acos((radiusTwoSquare + distanceSquare - radiusOneSquare) / (2 * radiusTwo * distance))) * 2;
  double a1 = 0.5 * theta * radiusTwoSquare - 0.5 * radiusTwoSquare * sin(theta);
  double a2 = 0.5 * phi * radiusOneSquare - 0.5 * radiusOneSquare * sin(phi);

  return a1 + a2;
}

/**************************************************
 * For operator classes/families
 **************************************************/



/**************************************************
 * Text operations
 **************************************************/

Datum gmt_text_compress(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum gmt_bpchar_compress(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum gmt_text_decompress(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum gmt_bpchar_decompress(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum gmt_text_consistent(PG_FUNCTION_ARGS)
{
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GMT_VARKEY* query = PG_GETARG_GMT_P(1);
  StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
  bool* recheck = (bool*) PG_GETARG_POINTER(4);
  GMT_VARKEY* key = DatumGetMtreeGist(entry->key);
  bool returnValue;
  int distance = string_distance(key->vl_dat, query->vl_dat);

  if (GIST_LEAF(entry))
  {
    *recheck = false;
    switch(strategy)
    {
      case SameStrategyNumber:
        returnValue = text_equals(key, query);
        break;
      case OverlapStrategyNumber:
        returnValue = text_overlap(key, query, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = text_contains(key, query, distance);
        break;
      case ContainedStrategyNumber:
        returnValue = text_contains(query, key, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %distance", strategy);
        break;
    }
  }
  else
  {
    switch(strategy)
    {
      case SameStrategyNumber:
        returnValue = text_contains(key, query, distance);
        *recheck = true;
        break;
      case OverlapStrategyNumber:
        returnValue = text_overlap(key, query, distance);
        *recheck = !text_contains(query, key, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = text_contains(key, query, distance);
        *recheck = true;
        break;
      case ContainedStrategyNumber:
        returnValue = text_overlap(key, query, distance);
        *recheck = text_contains(query, key, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %distance", strategy);
        break;
    }
  }

  PG_RETURN_BOOL(returnValue);
}

Datum gmt_bpchar_consistent(PG_FUNCTION_ARGS)
{
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GMT_VARKEY* query = PG_GETARG_GMT_P(1);
  StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
  bool* recheck = (bool*) PG_GETARG_POINTER(4);
  GMT_VARKEY* key = DatumGetMtreeGist(entry->key);
  bool returnValue;
  int distance = string_distance(key->vl_dat, query->vl_dat);

  if (GIST_LEAF(entry))
  {
    *recheck = false;
    switch(strategy)
    {
      case SameStrategyNumber:
        returnValue = text_equals(key, query);
        break;
      case OverlapStrategyNumber:
        returnValue = text_overlap(key, query, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = text_contains(key, query, distance);
        break;
      case ContainedStrategyNumber:
        returnValue = text_contains(query, key, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %distance", strategy);
        break;
    }
  }
  else
  {
    switch(strategy)
    {
      case SameStrategyNumber:
        returnValue = text_contains(key, query, distance);
        *recheck = true;
        break;
      case OverlapStrategyNumber:
        returnValue = text_overlap(key, query, distance);
        *recheck = !text_contains(query, key, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = text_contains(key, query, distance);
        *recheck = true;
        break;
      case ContainedStrategyNumber:
        returnValue = text_overlap(key, query, distance);
        *recheck = text_contains(query, key, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %distance", strategy);
        break;
    }
  }

  PG_RETURN_BOOL(returnValue);
}

Datum gmt_text_union(PG_FUNCTION_ARGS)
{
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GISTENTRY* entry = entryVector->vector;
  int ranges = entryVector->n;

  GMT_VARKEY* entries[ranges];
  for (int i = 0; i < ranges; ++i) {
    entries[i] = DatumGetMtreeGist(entry[i].key);
  }

  int searchRange;

  switch(unionStrategy) {
    case First:
      searchRange = 1;
      break;
    case Best:
      searchRange = ranges;
      break;
    default:
      elog(ERROR, "Invalid union strategy: %distance", unionStrategy);
      break;
  }

  int coveringRadii[searchRange];

  for (int i = 0; i < searchRange; ++i) {
    coveringRadii[i] = 0;

    for (int j = 0; j < ranges; ++j) {
      int distance = string_distance(entries[i]->vl_dat, entries[j]->vl_dat);
      int newCoveringRadius = distance + entries[j]->coveringRadius;

      if (coveringRadii[i] < newCoveringRadius) {
        coveringRadii[i] = newCoveringRadius;
      }
    }
  }

  int minimumIndex = 0;

  for (int i = 1; i < searchRange; ++i) {
    if (coveringRadii[i] < coveringRadii[minimumIndex]) {
      minimumIndex = i;
    }
  }

  GMT_VARKEY* out = copy_text(entries[minimumIndex]);
  out->coveringRadius = coveringRadii[minimumIndex];

  PG_RETURN_GMT_P(out);
}

Datum gmt_text_picksplit(PG_FUNCTION_ARGS)
{
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GIST_SPLITVEC* vector = (GIST_SPLITVEC*) PG_GETARG_POINTER(1);
  OffsetNumber maxOffset = entryVector->n - 1;
  OffsetNumber numberBytes = (maxOffset + 1) * sizeof(OffsetNumber);
  OffsetNumber* left;
  OffsetNumber* right;

  vector->spl_left = (OffsetNumber*) palloc(numberBytes);
  left = vector->spl_left;
  vector->spl_nleft = 0;

  vector->spl_right = (OffsetNumber*) palloc(numberBytes);
  right = vector->spl_right;
  vector->spl_nright = 0;

  GMT_VARKEY* entries[maxOffset];
  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    entries[i - FirstOffsetNumber] = DatumGetMtreeGist(entryVector->vector[i].key);
  }

  int distances[maxOffset][maxOffset];
  init_matrix(maxOffset, distances);

  int leftIndex, rightIndex, leftCandidateIndex, rightCandidateIndex;
  int trialCount = 100;
  int maxDistance = -1;
  int minCoveringSum = -1;
  int minCoveringMax = -1;
  int minOverlapArea = -1;
  int minSumArea = -1;

  switch (picksplitStrategy) {
    case Random:
      leftIndex = (int) random() % (maxOffset - 1);
      rightIndex = (leftIndex + 1) + (random() % (maxOffset - leftIndex - 1));
      break;
    case FirstTwo:
      leftIndex = 0;
      rightIndex = 1;
      break;
    case MaxDistanceFromFirst:
      maxDistance = -1;
      for (int r = 0; r < maxOffset; ++r) {
        int distance = get_matrix_distance(maxOffset, entries, distances, 0, r);
        if (distance > maxDistance) {
          maxDistance = distance;
          rightCandidateIndex = r;
        }
      }
      leftIndex = 0;
      rightIndex = rightCandidateIndex;
      break;
    case MaxDistancePair:
      for (OffsetNumber l = 0; l < maxOffset; ++l) {
        for (OffsetNumber r = l; r < maxOffset; ++r) {
          int distance = get_matrix_distance(maxOffset, entries, distances, l, r);
          if (distance > maxDistance) {
            maxDistance = distance;
            leftCandidateIndex = l;
            rightCandidateIndex = r;
          }
        }
      }
      leftIndex = 0;
      rightIndex = rightCandidateIndex;
      break;
    case SamplingMinCoveringSum:
      for (int i = 0; i < trialCount; ++i) {
        leftCandidateIndex = random() % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_matrix_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_matrix_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

          if (leftDistance < rightDistance) {
            if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
              leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
            }
          }
          else {
            if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
              rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
            }
          }
        }

        if (minCoveringSum == -1 || leftRadius + rightRadius < minCoveringSum) {
          minCoveringSum = leftRadius + rightRadius;
          leftIndex = leftCandidateIndex;
          rightIndex = rightCandidateIndex;
        }
      }
      break;
    case SamplingMinCoveringMax:
      for (int i = 0; i < trialCount; ++i) {
        leftCandidateIndex = random() % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; ++currentIndex) {
          int leftDistance = get_matrix_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_matrix_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

          if (leftDistance < rightDistance) {
            if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
              leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
            }
          }
          else {
            if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
              rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
            }
          }
        }

        if (minCoveringMax == -1 || MAX2(leftRadius, rightRadius) < minCoveringMax) {
          minCoveringMax = MAX2(leftRadius, rightRadius);
          leftIndex = leftCandidateIndex;
          rightIndex = rightCandidateIndex;
        }
      }
      break;
    case SamplingMinOverlapArea:
      for (int i = 0; i < trialCount; i++) {
        leftCandidateIndex = random() % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxOffset - leftCandidateIndex - 1));
        int distance = get_matrix_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_matrix_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_matrix_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

          if (leftDistance < rightDistance) {
            if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
              leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
            }
          }
          else {
            if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
              rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
            }
          }
        }

        double currentOverlapArea = get_area_overlap(leftRadius, rightRadius, distance);
        if (minOverlapArea == -1 || currentOverlapArea < minOverlapArea) {
          minOverlapArea = currentOverlapArea;
          leftIndex = leftCandidateIndex;
          rightIndex = rightCandidateIndex;
        }
      }
      break;
    case SamplingMinAreaSum:
      for (int i = 0; i < trialCount; i++) {
        leftCandidateIndex = (int) random() % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_matrix_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_matrix_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

          if (leftDistance < rightDistance) {
            if (leftDistance + entries[currentIndex]->coveringRadius > leftRadius) {
              leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
            }
          }
          else {
            if (rightDistance + entries[currentIndex]->coveringRadius > rightRadius) {
              rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
            }
          }
        }

        int currentSumArea = leftRadius * leftRadius + rightRadius * rightRadius;
        if (minSumArea == -1 || currentSumArea < minSumArea) {
          minSumArea = currentSumArea;
          leftIndex = leftCandidateIndex;
          rightIndex = rightCandidateIndex;
        }
      }
      break;
    default:
      elog(ERROR, "Invalid picksplit strategy: %distance", picksplitStrategy);
      break;
  }

  GMT_VARKEY* unionLeft = copy_text(entries[leftIndex]);
  GMT_VARKEY* unionRight = copy_text(entries[rightIndex]);
  GMT_VARKEY* current;

  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    int distanceLeft = get_matrix_distance(maxOffset, entries, distances, leftIndex, i - 1);
    int distanceRight = get_matrix_distance(maxOffset, entries, distances, rightIndex, i - 1);
    current = entries[i - 1];

    if (distanceLeft < distanceRight) {
      if (distanceLeft + current->coveringRadius > unionLeft->coveringRadius) {
        unionLeft->coveringRadius = distanceLeft + current->coveringRadius;
      }
      *left = i;
      ++left;
      ++(vector->spl_nleft);
    }
    else {
      if (distanceRight + current->coveringRadius > unionRight->coveringRadius) {
        unionRight->coveringRadius = distanceRight + current->coveringRadius;
      }
      *right = i;
      ++right;
      ++(vector->spl_nright);
    }
  }

  vector->spl_ldatum = PointerGetDatum(unionLeft);
  vector->spl_rdatum = PointerGetDatum(unionRight);

  PG_RETURN_POINTER(vector);
}

Datum gmt_text_same(PG_FUNCTION_ARGS)
{
  GMT_VARKEY* first = PG_GETARG_GMT_P(0);
  GMT_VARKEY* second = PG_GETARG_GMT_P(1);
  bool result = text_equals(first, second);

  PG_RETURN_BOOL(result);
}

Datum gmt_text_penalty(PG_FUNCTION_ARGS)
{
  GISTENTRY* originalEntry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GISTENTRY* newEntry = (GISTENTRY*) PG_GETARG_POINTER(1);
  float* penalty = (float*) PG_GETARG_POINTER(2);
  GMT_VARKEY* original = DatumGetMtreeGist(originalEntry->key);
  GMT_VARKEY* new = DatumGetMtreeGist(newEntry->key);

  int distance = string_distance(original->vl_dat, new->vl_dat);
  int newCoveringRadius = distance + new->coveringRadius;

  if (newCoveringRadius < original->coveringRadius)
  {
    *penalty = 0;
  }
  else
  {
    *penalty = (float) (newCoveringRadius - original->coveringRadius);
  }

  PG_RETURN_POINTER(penalty);
}
