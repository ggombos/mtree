#include "postgres.h"
#include "c.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"
#include "utils/elog.h"
#include "utils/palloc.h"
#include "access/genam.h"
#include "access/gist.h"
#include "access/gist_private.h"
#include "access/skey.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <utils/array.h>
#include <utils/lsyscache.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef struct {
  int parentDistance;
  int coveringRadius;
  char vl_length[4];
  char vl_data[FLEXIBLE_ARRAY_MEMBER];
} music_gist;

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
//PicksplitStrategy picksplitStrategy = %picksplitStrategy%;
PicksplitStrategy picksplitStrategy = Random;

#define SameStrategyNumber      1 //  =
#define OverlapStrategyNumber   2 // #&#
#define ContainsStrategyNumber  3 // #>#
#define ContainedStrategyNumber 4 // #<#
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define OWNHDRSZ (3 * sizeof(int))
#define DatumGetMusGist(x) ((music_gist *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MUS_GIST_P(x) DatumGetMusGist(PG_GETARG_DATUM(x))
#define PG_RETURN_MUS_GIST_P(x) PG_RETURN_POINTER(x)

PG_FUNCTION_INFO_V1(music_gist_input);
PG_FUNCTION_INFO_V1(music_gist_output);

PG_FUNCTION_INFO_V1(music_gist_consistent);
PG_FUNCTION_INFO_V1(gist_union);
PG_FUNCTION_INFO_V1(music_gist_same);
PG_FUNCTION_INFO_V1(music_gist_penalty);
PG_FUNCTION_INFO_V1(gist_picksplit);
PG_FUNCTION_INFO_V1(music_gist_compress);
PG_FUNCTION_INFO_V1(music_gist_decompress);
PG_FUNCTION_INFO_V1(music_gist_distance_internal);

PG_FUNCTION_INFO_V1(music_gist_distance);
PG_FUNCTION_INFO_V1(music_gist_overlap);
PG_FUNCTION_INFO_V1(music_gist_contains);
PG_FUNCTION_INFO_V1(music_gist_contained);
PG_FUNCTION_INFO_V1(music_gist_same_internal);

PG_FUNCTION_INFO_V1(text_distance);

Datum music_gist_input(PG_FUNCTION_ARGS) {
  char* string = PG_GETARG_CSTRING(0);
  int coveringRadius = 0;

  if (isdigit(string[0])) {
    char* number = string;

    while (isdigit(string[0])) {
      ++string;
    }

    string[0] = '\0';
    ++string;
    coveringRadius = atoi(number);
  }

  int stringLength = strlen(string);
  music_gist* result = (music_gist*) palloc(OWNHDRSZ + stringLength * sizeof(char) + 1);
  result->coveringRadius = coveringRadius;
  result->parentDistance = 0;

  SET_VARSIZE(result, OWNHDRSZ + stringLength * sizeof(char) + 1);

  strcpy(result->vl_data, string);
  result->vl_data[stringLength] = '\0';

  PG_RETURN_POINTER(result);
}

Datum music_gist_output(PG_FUNCTION_ARGS) {
  music_gist* music = PG_GETARG_MUS_GIST_P(0);
  char* result;

  if (music->coveringRadius == 0) {
    result = psprintf("%s", music->vl_data);
  }
  else {
    result = psprintf("%distance|%s", music->coveringRadius, music->vl_data);
  }

  PG_RETURN_CSTRING(result);
}

bool internal_music_gist_same(music_gist* first, music_gist* second) {
  return !strcmp(first->vl_data, second->vl_data);
}

Datum music_gist_same(PG_FUNCTION_ARGS) {
  music_gist* first = PG_GETARG_MUS_GIST_P(0);
  music_gist* second = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_music_gist_same(first, second);
  PG_RETURN_BOOL(result);
}

int string_distance(char* a, char* b) {
  int lengthOfA = strlen(a);
  int lengthOfB = strlen(b);
  int x, y, lastDiagonal, oldDiagonal;
  int column[lengthOfA + 1];

  column[0] = 0;
  for (y = 1; y <= lengthOfA; ++y) {
    column[y] = y;
  }

  for (x = 1; x <= lengthOfB; ++x) {
    column[0] = x;
    for (y = 1, lastDiagonal = x - 1; y <= lengthOfA; ++y) {
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

int internal_music_gist_distance(music_gist* first, music_gist* second) {
  char* firstData = first->vl_data;
  char* secondData = second->vl_data;

  return string_distance(firstData, secondData);
}

Datum music_gist_distance(PG_FUNCTION_ARGS) {
  music_gist* first = PG_GETARG_MUS_GIST_P(0);
  music_gist* second = PG_GETARG_MUS_GIST_P(1);

  PG_RETURN_INT32(internal_music_gist_distance(first, second));
}

int internal_text_distance(text* first, text* second) {
  return string_distance(first->vl_dat, second->vl_dat);
}

Datum text_distance(PG_FUNCTION_ARGS) {
  text* first = PG_GETARG_TEXT_P(0);
  text* second = PG_GETARG_TEXT_P(1);

  PG_RETURN_INT32(internal_text_distance(first, second));
}

bool internal_music_gist_overlap_d(music_gist* first, music_gist* second, int distance) {
  return distance <= first->coveringRadius + second->coveringRadius;
}

bool internal_music_gist_overlap(music_gist* first, music_gist* second) {
  return internal_music_gist_overlap_d(first, second, internal_music_gist_distance(first, second));
}

Datum music_gist_overlap(PG_FUNCTION_ARGS) {
  music_gist* first = PG_GETARG_MUS_GIST_P(0);
  music_gist* second = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_music_gist_overlap(first, second);

  PG_RETURN_BOOL(result);
}

bool contains_d(music_gist* big, music_gist* small, int distance) {
  return big->coveringRadius >= distance + small->coveringRadius;
}

bool contains(music_gist* big, music_gist* small) {
  return contains_d(big, small, internal_music_gist_distance(big, small));
}

bool internal_music_gist_contains_d(music_gist* first, music_gist* second, int distance) {
  return contains_d(first, second, distance);
}

bool internal_music_gist_contains(music_gist* first, music_gist* second) {
  return contains(first, second);
}

Datum music_gist_contains(PG_FUNCTION_ARGS) {
  music_gist* first = PG_GETARG_MUS_GIST_P(0);
  music_gist* second = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_music_gist_contains(first, second);

  PG_RETURN_BOOL(result);
}

bool internal_music_gist_contained_d(music_gist* first, music_gist* second, int distance) {
  return contains_d(second, first, distance);
}

bool internal_music_gist_contained(music_gist* first, music_gist* second) {
  return contains(second, first);
}

Datum music_gist_contained(PG_FUNCTION_ARGS) {
  music_gist* first = PG_GETARG_MUS_GIST_P(0);
  music_gist* second = PG_GETARG_MUS_GIST_P(1);
  bool result = internal_music_gist_contained(first, second);

  PG_RETURN_BOOL(result);
}

Datum music_gist_compress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum music_gist_decompress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum music_gist_consistent(PG_FUNCTION_ARGS) {
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  music_gist* query = PG_GETARG_MUS_GIST_P(1);
  StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
  bool* recheck = (bool*) PG_GETARG_POINTER(4);
  music_gist* key = DatumGetMusGist(entry->key);
  bool returnValue;
  int distance = internal_music_gist_distance(key, query);

  if (GIST_LEAF(entry)) {
    *recheck = false;
    switch(strategy) {
      case SameStrategyNumber:
        returnValue = internal_music_gist_same(key, query);
        break;
      case OverlapStrategyNumber:
        returnValue = internal_music_gist_overlap_d(key, query, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = internal_music_gist_contains_d(key, query, distance);
        break;
      case ContainedStrategyNumber:
        returnValue = internal_music_gist_contained_d(key, query, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %distance", strategy);
        break;
    }
  }
  else {
    switch(strategy) {
      case SameStrategyNumber:
        returnValue = internal_music_gist_contains_d(key, query, distance);
        *recheck = true;
        break;
      case OverlapStrategyNumber:
        returnValue = internal_music_gist_overlap_d(key, query, distance);
        *recheck = !internal_music_gist_contained_d(key, query, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = internal_music_gist_contains_d(key, query, distance);
        *recheck = true;
        break;
      case ContainedStrategyNumber:
        returnValue = internal_music_gist_overlap_d(key, query, distance);
        *recheck = !internal_music_gist_contained_d(key, query, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %distance", strategy);
        break;
    }
  }

  PG_RETURN_BOOL(returnValue);
}

void initDistances(int size, int distances[size][size]) {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      distances[i][j] = -1;
    }
  }
}

int getDistance(int size, music_gist *entries[size], int distances[size][size], int i, int j) {
  if (distances[i][j] == -1) {
    distances[i][j] = internal_music_gist_distance(entries[i], entries[j]);
  }
  return distances[i][j];
}

music_gist* music_gist_deep_copy(music_gist *source) {
  music_gist* destination = (music_gist*) palloc(VARSIZE_ANY(source));

  memcpy(destination, source, VARSIZE_ANY(source));

  return destination;
}

Datum gist_union(PG_FUNCTION_ARGS) {
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GISTENTRY* entry = entryVector->vector;
  int ranges = entryVector->n;

  music_gist* entries[ranges];
  for (int i = 0; i < ranges; ++i) {
    entries[i] = DatumGetMusGist(entry[i].key);
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
      int distance = internal_music_gist_distance(entries[i], entries[j]);
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

  music_gist* out = music_gist_deep_copy(entries[minimumIndex]);
  out->coveringRadius = coveringRadii[minimumIndex];

  PG_RETURN_MUS_GIST_P(out);
}

Datum music_gist_penalty(PG_FUNCTION_ARGS) {
  GISTENTRY* originalEntry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GISTENTRY* newEntry = (GISTENTRY*) PG_GETARG_POINTER(1);
  float* penalty = (float*) PG_GETARG_POINTER(2);
  music_gist* original = DatumGetMusGist(originalEntry->key);
  music_gist* new = DatumGetMusGist(newEntry->key);

  int distance = internal_music_gist_distance(original, new);
  int newCoveringRadius = distance + new->coveringRadius;
  *penalty = (newCoveringRadius < original->coveringRadius ? 0 : newCoveringRadius - original->coveringRadius);

  PG_RETURN_POINTER(penalty);
}

double overlapArea(int radiusOne, int radiusTwo, int distance) {
  if (radiusOne == 0 || radiusTwo == 0 || distance == 0) {
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

Datum gist_picksplit(PG_FUNCTION_ARGS) {
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

  music_gist* entries[maxOffset];
  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    entries[i - FirstOffsetNumber] = DatumGetMusGist(entryVector->vector[i].key);
  }

  int distances[maxOffset][maxOffset];
  initDistances(maxOffset, distances);

  int leftIndex, rightIndex, leftCandidateIndex, rightCandidateIndex;
  int trialCount = 100;
  int maxDistance = -1;
  int minCoveringSum = -1;
  int minCoveringMax = -1;
  int minOverlapArea = -1;
  int minSumArea = -1;

  switch (picksplitStrategy) {
    case Random:
      leftIndex = random() % (maxOffset - 1);
      rightIndex = (leftIndex + 1) + (random() % (maxOffset - leftIndex - 1));
      break;
    case FirstTwo:
      leftIndex = 0;
      rightIndex = 1;
      break;
    case MaxDistanceFromFirst:
      maxDistance = -1;
      for (int r = 0; r < maxOffset; ++r) {
        int distance = getDistance(maxOffset, entries, distances, 0, r);
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
          int distance = getDistance(maxOffset, entries, distances, l, r);
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
          int leftDistance = getDistance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = getDistance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
          int leftDistance = getDistance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = getDistance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
        int distance = getDistance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = getDistance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = getDistance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

        double currentOverlapArea = overlapArea(leftRadius, rightRadius, distance);
        if (minOverlapArea == -1 || currentOverlapArea < minOverlapArea) {
          minOverlapArea = currentOverlapArea;
          leftIndex = leftCandidateIndex;
          rightIndex = rightCandidateIndex;
        }
      }
      break;
    case SamplingMinAreaSum:
      for (int i = 0; i < trialCount; i++) {
        leftCandidateIndex = random() % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (random() % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = getDistance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = getDistance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

  music_gist* unionLeft = music_gist_deep_copy(entries[leftIndex]);
  music_gist* unionRight = music_gist_deep_copy(entries[rightIndex]);
  music_gist* current;

  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    int distanceLeft = getDistance(maxOffset, entries, distances, leftIndex, i - 1);
    int distanceRight = getDistance(maxOffset, entries, distances, rightIndex, i - 1);
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

Datum music_gist_same_internal(PG_FUNCTION_ARGS) {
  music_gist* first = (music_gist*) PG_GETARG_POINTER(0);
  music_gist* second = (music_gist*) PG_GETARG_POINTER(1);
  bool* result = (bool*) PG_GETARG_POINTER(2);

  *result = (first->vl_data[0] == second->vl_data[0]);
  PG_RETURN_POINTER(result);
}

Datum music_gist_distance_internal(PG_FUNCTION_ARGS) {
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  music_gist* query = PG_GETARG_MUS_GIST_P(1);
  music_gist* key = DatumGetMusGist(entry->key);
  double returnValue = internal_music_gist_distance(query, key);

  PG_RETURN_FLOAT8(returnValue);
}
