/*
 * contrib/mtree_gist/mtree_text.c
 */

#include "mtree_text.h"

#include "mtree_text_util.h"
#include "mtree_util.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

/* TODO: Strategy should be a parameter! */
const UnionStrategy UNION_STRATEGY = Best;
const PicksplitStrategy PICKSPLIT_STRATEGY = SamplingMinOverlapArea;

PG_FUNCTION_INFO_V1(mtree_text_input);
PG_FUNCTION_INFO_V1(mtree_text_output);

PG_FUNCTION_INFO_V1(mtree_text_consistent);
PG_FUNCTION_INFO_V1(mtree_text_union);
PG_FUNCTION_INFO_V1(mtree_text_same);

PG_FUNCTION_INFO_V1(mtree_text_penalty);
PG_FUNCTION_INFO_V1(mtree_text_picksplit);

PG_FUNCTION_INFO_V1(mtree_text_compress);
PG_FUNCTION_INFO_V1(mtree_text_decompress);
PG_FUNCTION_INFO_V1(mtree_text_distance_float);

PG_FUNCTION_INFO_V1(mtree_text_distance);
PG_FUNCTION_INFO_V1(mtree_text_overlap);
PG_FUNCTION_INFO_V1(mtree_text_contains);
PG_FUNCTION_INFO_V1(mtree_text_contained);
PG_FUNCTION_INFO_V1(mtree_text_equals_first);

Datum mtree_text_input(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_input"); */
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

  size_t stringLength = strlen(string);
  mtree_text* result = (mtree_text*) palloc(OWNHDRSZ + stringLength * sizeof(char) + 1);
  result->coveringRadius = coveringRadius;
  result->parentDistance = 0;

  SET_VARSIZE(result, OWNHDRSZ + stringLength * sizeof(char) + 1);

  strcpy(result->vl_data, string);
  result->vl_data[stringLength] = '\0';

  PG_RETURN_POINTER(result);
}

Datum mtree_text_output(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_output"); */
  mtree_text* text = PG_GETARG_MTREE_TEXT_P(0);
  char* result;

  if (text->coveringRadius == 0) {
    result = psprintf("%s", text->vl_data);
  } else {
    result = psprintf("distance|%d data|%s", text->coveringRadius, text->vl_data);
  }

  PG_RETURN_CSTRING(result);
}

Datum mtree_text_consistent(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_consistent"); */

  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  mtree_text* query = PG_GETARG_MTREE_TEXT_P(1);
  StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);
  bool* recheck = (bool*) PG_GETARG_POINTER(4);
  mtree_text* key = DatumGetMtreeText(entry->key);
  bool returnValue;
  int distance = mtree_text_string_distance(key, query);

  if (GIST_LEAF(entry)) {
    *recheck = false;
    switch(strategy) {
      case SameStrategyNumber:
        returnValue = mtree_text_equals(key, query);
        break;
      case OverlapStrategyNumber:
        returnValue = mtree_text_overlap_distance(key, query, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = mtree_text_contains_distance(key, query, distance);
        break;
      case ContainedStrategyNumber:
        returnValue = mtree_text_contained_distance(key, query, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %d", strategy);
        break;
    }
  } else {
    switch(strategy) {
      case SameStrategyNumber:
        returnValue = mtree_text_contains_distance(key, query, distance);
        *recheck = true;
        break;
      case OverlapStrategyNumber:
        returnValue = mtree_text_overlap_distance(key, query, distance);
        *recheck = !mtree_text_contained_distance(key, query, distance);
        break;
      case ContainsStrategyNumber:
        returnValue = mtree_text_contains_distance(key, query, distance);
        *recheck = true;
        break;
      case ContainedStrategyNumber:
        returnValue = mtree_text_overlap_distance(key, query, distance);
        *recheck = !mtree_text_contained_distance(key, query, distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategy: %d", strategy);
        break;
    }
  }

  PG_RETURN_BOOL(returnValue);
}

Datum mtree_text_union(PG_FUNCTION_ARGS) {
  // elog(INFO, "mtree_text_union");
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GISTENTRY* entry = entryVector->vector;
  int ranges = entryVector->n;

  mtree_text* entries[ranges];
  for (int i = 0; i < ranges; ++i) {
    entries[i] = DatumGetMtreeText(entry[i].key);
  }

  int searchRange;

  switch(UNION_STRATEGY) {
    case First:
      searchRange = 1;
      break;
    case Best:
      searchRange = ranges;
      break;
    default:
      elog(ERROR, "Invalid union strategy: %d", UNION_STRATEGY);
      break;
  }

  int coveringRadii[searchRange];

  for (int i = 0; i < searchRange; ++i) {
    coveringRadii[i] = 0;

    for (int j = 0; j < ranges; ++j) {
      int distance = mtree_text_string_distance(entries[i], entries[j]);
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

  mtree_text* out = mtree_text_deep_copy(entries[minimumIndex]);
  out->coveringRadius = coveringRadii[minimumIndex];

  PG_RETURN_MTREE_TEXT_P(out);
}

Datum mtree_text_same(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_same"); */
  mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
  mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);
  bool result = mtree_text_equals(first, second);
  PG_RETURN_BOOL(result);
}

Datum mtree_text_penalty(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_penalty"); */
  GISTENTRY* originalEntry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GISTENTRY* newEntry = (GISTENTRY*) PG_GETARG_POINTER(1);
  float* penalty = (float*) PG_GETARG_POINTER(2);
  mtree_text* original = DatumGetMtreeText(originalEntry->key);
  mtree_text* new = DatumGetMtreeText(newEntry->key);

  int distance = mtree_text_string_distance(original, new);
  int newCoveringRadius = distance + new->coveringRadius;
  *penalty = (float) (newCoveringRadius < original->coveringRadius ? 0 : newCoveringRadius - original->coveringRadius);

  PG_RETURN_POINTER(penalty);
}

Datum mtree_text_picksplit(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_picksplit"); */
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GIST_SPLITVEC* vector = (GIST_SPLITVEC*) PG_GETARG_POINTER(1);
  OffsetNumber maxOffset = (OffsetNumber) entryVector->n - 1;
  OffsetNumber numberBytes = (OffsetNumber) (maxOffset + 1) * sizeof(OffsetNumber);
  OffsetNumber* left;
  OffsetNumber* right;

  vector->spl_left = (OffsetNumber*) palloc(numberBytes);
  left = vector->spl_left;
  vector->spl_nleft = 0;

  vector->spl_right = (OffsetNumber*) palloc(numberBytes);
  right = vector->spl_right;
  vector->spl_nright = 0;

  mtree_text* entries[maxOffset];
  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    entries[i - FirstOffsetNumber] = DatumGetMtreeText(entryVector->vector[i].key);
  }

  int distances[maxOffset][maxOffset];
  init_distances(maxOffset, *distances);

  int leftIndex, rightIndex, leftCandidateIndex, rightCandidateIndex;
  int trialCount = 100;
  int maxDistance = -1;
  int minCoveringSum = -1;
  int minCoveringMax = -1;
  int minOverlapArea = -1;
  int minSumArea = -1;

  switch (PICKSPLIT_STRATEGY) {
    case Random:
      leftIndex = ((int) random()) % (maxOffset - 1);
      rightIndex = (leftIndex + 1) + (((int) random()) % (maxOffset - leftIndex - 1));
      break;
    case FirstTwo:
      leftIndex = 0;
      rightIndex = 1;
      break;
    case MaxDistanceFromFirst:
      maxDistance = -1;
      for (int r = 0; r < maxOffset; ++r) {
        int distance = get_distance(maxOffset, entries, distances, 0, r);
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
          int distance = get_distance(maxOffset, entries, distances, l, r);
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
        leftCandidateIndex = ((int) random()) % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (((int) random()) % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
        leftCandidateIndex = ((int) random()) % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (((int) random()) % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; ++currentIndex) {
          int leftDistance = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
        leftCandidateIndex = ((int) random()) % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (((int) random()) % (maxOffset - leftCandidateIndex - 1));
        int distance = get_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

        double currentOverlapArea = overlap_area(leftRadius, rightRadius, distance);
        if (minOverlapArea == -1 || currentOverlapArea < minOverlapArea) {
          minOverlapArea = (int) currentOverlapArea;
          leftIndex = leftCandidateIndex;
          rightIndex = rightCandidateIndex;
        }
      }
      break;
    case SamplingMinAreaSum:
      for (int i = 0; i < trialCount; i++) {
        leftCandidateIndex = ((int) random()) % (maxOffset - 1);
        rightCandidateIndex = (leftCandidateIndex + 1) + (((int) random()) % (maxOffset - leftCandidateIndex - 1));
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
      elog(ERROR, "Invalid picksplit strategy: %distance", PICKSPLIT_STRATEGY);
      break;
  }

  mtree_text* unionLeft = mtree_text_deep_copy(entries[leftIndex]);
  mtree_text* unionRight = mtree_text_deep_copy(entries[rightIndex]);
  mtree_text* current;

  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    int distanceLeft = get_distance(maxOffset, entries, distances, leftIndex, i - 1);
    int distanceRight = get_distance(maxOffset, entries, distances, rightIndex, i - 1);
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

Datum mtree_text_compress(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_compress"); */
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_text_decompress(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_decompress"); */
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_text_distance_float(PG_FUNCTION_ARGS) {
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  mtree_text* query = PG_GETARG_MTREE_TEXT_P(1);
  mtree_text* key = DatumGetMtreeText(entry->key);
  bool isLeaf = GistPageIsLeaf(entry->page);
  bool *recheck = (bool *) PG_GETARG_POINTER(4);

  if (isLeaf) {
	  *recheck = true;
  }

  PG_RETURN_FLOAT4(mtree_text_string_distance(query, key));
}

Datum mtree_text_distance(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_distance"); */
  mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
  mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);

  /*
    leaf     -> return distance
    non-leaf -> return distance to closest children
  */

  PG_RETURN_FLOAT4(mtree_text_string_distance(first, second));
}

Datum mtree_text_overlap(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_overlap"); */
  mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
  mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);
  bool result = mtree_text_overlap_wrapper(first, second);

  PG_RETURN_BOOL(result);
}

Datum mtree_text_contains(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_contains"); */
  mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
  mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);
  bool result = mtree_text_contains_wrapper(first, second);

  PG_RETURN_BOOL(result);
}

Datum mtree_text_contained(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_contained"); */
  mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
  mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);
  bool result = mtree_text_contained_wrapper(first, second);

  PG_RETURN_BOOL(result);
}

Datum mtree_text_equals_first(PG_FUNCTION_ARGS) {
  /* elog(INFO, "mtree_text_equals_first"); */
  mtree_text* first = (mtree_text*) PG_GETARG_POINTER(0);
  mtree_text* second = (mtree_text*) PG_GETARG_POINTER(1);
  bool* result = (bool*) PG_GETARG_POINTER(2);

  *result = (first->vl_data[0] == second->vl_data[0]);
  PG_RETURN_POINTER(result);
}
