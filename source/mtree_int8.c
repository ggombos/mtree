/*
 * contrib/mtree_gist/mtree_int8.c
 */

#include "mtree_int8.h"

#include "mtree_int8_util.h"
#include "mtree_util.h"

/* TODO: Strategy should be a parameter! */
const UnionStrategy UNION_STRATEGY_INT8 = Best;
const PicksplitStrategy PICKSPLIT_STRATEGY_INT8 = SamplingMinOverlapArea;

PG_FUNCTION_INFO_V1(mtree_int8_input);
PG_FUNCTION_INFO_V1(mtree_int8_output);

PG_FUNCTION_INFO_V1(mtree_int8_consistent);
PG_FUNCTION_INFO_V1(mtree_int8_union);
PG_FUNCTION_INFO_V1(mtree_int8_same);

PG_FUNCTION_INFO_V1(mtree_int8_penalty);
PG_FUNCTION_INFO_V1(mtree_int8_picksplit);

PG_FUNCTION_INFO_V1(mtree_int8_compress);
PG_FUNCTION_INFO_V1(mtree_int8_decompress);

PG_FUNCTION_INFO_V1(mtree_int8_distance);

PG_FUNCTION_INFO_V1(mtree_int8_contains_operator);
PG_FUNCTION_INFO_V1(mtree_int8_contained_operator);
PG_FUNCTION_INFO_V1(mtree_int8_distance_operator);
PG_FUNCTION_INFO_V1(mtree_int8_overlap_operator);

Datum mtree_int8_input(PG_FUNCTION_ARGS) {
  char* input = PG_GETARG_CSTRING(0);

  mtree_int8* result = (mtree_int8*) palloc(MTREE_INT8_SIZE);
  result->coveringRadius = 0;
  result->parentDistance = 0;

  SET_VARSIZE(result, MTREE_INT8_SIZE);

  char* tmp;
  result->data = strtoul(input, &tmp, 10);

  PG_RETURN_POINTER(result);
}

Datum mtree_int8_output(PG_FUNCTION_ARGS) {
  mtree_int8* output = PG_GETARG_MTREE_INT8_P(0);
  char* result;

  if (output->coveringRadius == 0) {
    result = psprintf("%ld", output->data);
  } else {
    result = psprintf(
      "coveringRadius|%d parentDistance|%d data|%ld",
      output->coveringRadius, output->parentDistance, output->data
    );
  }

  PG_RETURN_CSTRING(result);
}

Datum mtree_int8_consistent(PG_FUNCTION_ARGS) {
  GISTENTRY *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
  mtree_int8* query = PG_GETARG_MTREE_INT8_P(1);
  StrategyNumber strategyNumber = (StrategyNumber) PG_GETARG_UINT16(2);
  bool *recheck = (bool *) PG_GETARG_POINTER(4);
  mtree_int8* key = DatumGetMtreeInt8(entry->key);
  int distance = mtree_int8_distance_internal(key, query);

  bool returnValue;
  if (GIST_LEAF(entry)) {
    *recheck = false;
    switch(strategyNumber) {
      case SameStrategyNumber:
        returnValue = mtree_int8_equals(key, query);
        break;
      case OverlapStrategyNumber:
        returnValue = mtree_int8_overlap_distance(key, query, &distance);
        break;
      case ContainsStrategyNumber:
        returnValue = mtree_int8_contains_distance(key, query, &distance);
        break;
      case ContainedStrategyNumber:
        returnValue = mtree_int8_contained_distance(key, query, &distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategyNumber: %d", strategyNumber);
        break;
    }
  } else {
    switch(strategyNumber) {
      case SameStrategyNumber:
        returnValue = mtree_int8_contains_distance(key, query, &distance);
        *recheck = true;
        break;
      case OverlapStrategyNumber:
        returnValue = mtree_int8_overlap_distance(key, query, &distance);
        *recheck = !mtree_int8_contained_distance(key, query, &distance);
        break;
      case ContainsStrategyNumber:
        returnValue = mtree_int8_contains_distance(key, query, &distance);
        *recheck = true;
        break;
      case ContainedStrategyNumber:
        returnValue = mtree_int8_overlap_distance(key, query, &distance);
        *recheck = !mtree_int8_contained_distance(key, query, &distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategyNumber: %d", strategyNumber);
        break;
    }
  }

  PG_RETURN_BOOL(returnValue);
}

Datum mtree_int8_union(PG_FUNCTION_ARGS) {
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GISTENTRY* entry = entryVector->vector;
  int ranges = entryVector->n;

  mtree_int8* entries[ranges];
  for (int i = 0; i < ranges; ++i) {
    entries[i] = DatumGetMtreeInt8(entry[i].key);
  }

  int searchRange;

  switch(UNION_STRATEGY_INT8) {
    case First:
      searchRange = 1;
      break;
    case Best:
      searchRange = ranges;
      break;
    default:
      elog(ERROR, "Invalid union strategy: %d", UNION_STRATEGY_INT8);
      break;
  }

  int coveringRadii[searchRange];

  for (int i = 0; i < searchRange; ++i) {
    coveringRadii[i] = 0;

    for (int j = 0; j < ranges; ++j) {
      int distance = mtree_int8_distance_internal(entries[i], entries[j]);
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

  mtree_int8* out = mtree_int8_deep_copy(entries[minimumIndex]);
  out->coveringRadius = coveringRadii[minimumIndex];

  PG_RETURN_MTREE_INT8_P(out);
}

Datum mtree_int8_same(PG_FUNCTION_ARGS) {
  mtree_int8* first = PG_GETARG_MTREE_INT8_P(0);
  mtree_int8* second = PG_GETARG_MTREE_INT8_P(1);
  PG_RETURN_BOOL(mtree_int8_equals(first, second));
}

Datum mtree_int8_penalty(PG_FUNCTION_ARGS) {
  GISTENTRY* originalEntry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GISTENTRY* newEntry = (GISTENTRY*) PG_GETARG_POINTER(1);
  float* penalty = (float*) PG_GETARG_POINTER(2);
  mtree_int8* original = DatumGetMtreeInt8(originalEntry->key);
  mtree_int8* new = DatumGetMtreeInt8(newEntry->key);

  int distance = mtree_int8_distance_internal(original, new);
  int newCoveringRadius = distance + new->coveringRadius;
  *penalty = (float) (newCoveringRadius < original->coveringRadius ? 0 : newCoveringRadius - original->coveringRadius);

  PG_RETURN_POINTER(penalty);
}

/* TODO: Lots of duplicate code. */
Datum mtree_int8_picksplit(PG_FUNCTION_ARGS) {
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

  mtree_int8* entries[maxOffset];
  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    entries[i - FirstOffsetNumber] = DatumGetMtreeInt8(entryVector->vector[i].key);
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

  switch (PICKSPLIT_STRATEGY_INT8) {
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
        int distance = get_int8_distance(maxOffset, entries, distances, 0, r);
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
          int distance = get_int8_distance(maxOffset, entries, distances, l, r);
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
          int leftDistance = get_int8_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_int8_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
          int leftDistance = get_int8_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_int8_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
        int distance = get_int8_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_int8_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_int8_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
          int leftDistance = get_int8_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_int8_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
      elog(ERROR, "Invalid picksplit strategy: %d", PICKSPLIT_STRATEGY_INT8);
      break;
  }

  mtree_int8* unionLeft = mtree_int8_deep_copy(entries[leftIndex]);
  mtree_int8* unionRight = mtree_int8_deep_copy(entries[rightIndex]);
  mtree_int8* current;

  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    int distanceLeft = get_int8_distance(maxOffset, entries, distances, leftIndex, i - 1);
    int distanceRight = get_int8_distance(maxOffset, entries, distances, rightIndex, i - 1);
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

Datum mtree_int8_compress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_int8_decompress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_int8_distance(PG_FUNCTION_ARGS) {
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  mtree_int8* query = PG_GETARG_MTREE_INT8_P(1);
  mtree_int8* key = DatumGetMtreeInt8(entry->key);
  bool isLeaf = GistPageIsLeaf(entry->page);
  bool *recheck = (bool *) PG_GETARG_POINTER(4);

  if (isLeaf) {
	  *recheck = true;
  }

  PG_RETURN_FLOAT4((float4) mtree_int8_distance_internal(query, key));
}

Datum mtree_int8_distance_operator(PG_FUNCTION_ARGS) {
  mtree_int8* first = PG_GETARG_MTREE_INT8_P(0);
  mtree_int8* second = PG_GETARG_MTREE_INT8_P(1);

  PG_RETURN_FLOAT4((float4) mtree_int8_distance_internal(first, second));
}

Datum mtree_int8_overlap_operator(PG_FUNCTION_ARGS) {
  mtree_int8* first = PG_GETARG_MTREE_INT8_P(0);
  mtree_int8* second = PG_GETARG_MTREE_INT8_P(1);
  bool result = mtree_int8_overlap_distance(first, second, mtree_int8_distance_internal(first, second));

  PG_RETURN_BOOL(result);
}

Datum mtree_int8_contains_operator(PG_FUNCTION_ARGS) {
  mtree_int8* first = PG_GETARG_MTREE_INT8_P(0);
  mtree_int8* second = PG_GETARG_MTREE_INT8_P(1);
  bool result = mtree_int8_contains_distance(first, second, mtree_int8_distance_internal(first, second));

  PG_RETURN_BOOL(result);
}

Datum mtree_int8_contained_operator(PG_FUNCTION_ARGS) {
  mtree_int8* first = PG_GETARG_MTREE_INT8_P(0);
  mtree_int8* second = PG_GETARG_MTREE_INT8_P(1);
  bool result = mtree_int8_contains_distance(second, first, mtree_int8_distance_internal(second, first));

  PG_RETURN_BOOL(result);
}
