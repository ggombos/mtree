/*
 * contrib/mtree_gist/mtree_text_array.c
 */

#include "mtree_text_array.h"

#include "mtree_text_array_util.h"

/* TODO: Strategy should be a parameter! */
const UnionStrategy UNION_STRATEGY_text_array = Best;
const PicksplitStrategy PICKSPLIT_STRATEGY_text_array = SamplingMinOverlapArea;

PG_FUNCTION_INFO_V1(mtree_text_array_input);
PG_FUNCTION_INFO_V1(mtree_text_array_output);

PG_FUNCTION_INFO_V1(mtree_text_array_consistent);
PG_FUNCTION_INFO_V1(mtree_text_array_union);
PG_FUNCTION_INFO_V1(mtree_text_array_same);

PG_FUNCTION_INFO_V1(mtree_text_array_penalty);
PG_FUNCTION_INFO_V1(mtree_text_array_picksplit);

PG_FUNCTION_INFO_V1(mtree_text_array_compress);
PG_FUNCTION_INFO_V1(mtree_text_array_decompress);

PG_FUNCTION_INFO_V1(mtree_text_array_distance);

PG_FUNCTION_INFO_V1(mtree_text_array_contains_operator);
PG_FUNCTION_INFO_V1(mtree_text_array_contained_operator);
PG_FUNCTION_INFO_V1(mtree_text_array_distance_operator);
PG_FUNCTION_INFO_V1(mtree_text_array_overlap_operator);

Datum mtree_text_array_input(PG_FUNCTION_ARGS) {
  char* input = PG_GETARG_CSTRING(0);

  // TODO: Validate input string syntax

  unsigned char arrayLength = get_array_length(input);

  size_t size = MTREE_TEXT_ARRAY_SIZE + arrayLength * sizeof(char) + 1;
  mtree_text_array* result = (mtree_text_array*) palloc(size);

  char* tmp;
  char* arrayElement = strtok(input, ",");
  for (unsigned char i = 0; i < arrayLength; ++i) {
    strcpy(result->data[i],arrayElement);
    arrayElement = strtok(NULL, ",");
  }

  result->coveringRadius = 0;
  result->parentDistance = 0;
  result->arrayLength = arrayLength;

  SET_VARSIZE(result, size);

  PG_RETURN_POINTER(result);
}

Datum mtree_text_array_output(PG_FUNCTION_ARGS) {
  mtree_text_array* output = PG_GETARG_MTREE_TEXT_ARRAY_P(0);

  unsigned char arrayLength = output->arrayLength;

  StringInfoData stringInfo;
  initStringInfo(&stringInfo);

  // TODO: Fix this, strange shit
  char tmp[20];
  for (unsigned char i = 0; i < arrayLength; ++i) {
    sprintf(tmp, "%s", output->data[i]);
    appendStringInfoString(&stringInfo, tmp);
    if (i != arrayLength - 1) {
      appendStringInfoChar(&stringInfo, ',');
    }
  }

  PG_RETURN_CSTRING(stringInfo.data);
}

Datum mtree_text_array_consistent(PG_FUNCTION_ARGS) {
  GISTENTRY *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
  mtree_text_array* query = PG_GETARG_MTREE_TEXT_ARRAY_P(1);
  StrategyNumber strategyNumber = (StrategyNumber) PG_GETARG_UINT16(2);
  bool *recheck = (bool *) PG_GETARG_POINTER(4);
  mtree_text_array* key = DatumGetMtreeTextArray(entry->key);
  int distance = mtree_text_array_distance_internal(key, query);

  bool returnValue;
  if (GIST_LEAF(entry)) {
    *recheck = false;
    switch(strategyNumber) {
      case SameStrategyNumber:
        returnValue = mtree_text_array_equals(key, query);
        break;
      case OverlapStrategyNumber:
        returnValue = mtree_text_array_overlap_distance(key, query, &distance);
        break;
      case ContainsStrategyNumber:
        returnValue = mtree_text_array_contains_distance(key, query, &distance);
        break;
      case ContainedStrategyNumber:
        returnValue = mtree_text_array_contained_distance(key, query, &distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategyNumber: %d", strategyNumber);
        break;
    }
  } else {
    switch(strategyNumber) {
      case SameStrategyNumber:
        returnValue = mtree_text_array_contains_distance(key, query, &distance);
        *recheck = true;
        break;
      case OverlapStrategyNumber:
        returnValue = mtree_text_array_overlap_distance(key, query, &distance);
        *recheck = !mtree_text_array_contained_distance(key, query, &distance);
        break;
      case ContainsStrategyNumber:
        returnValue = mtree_text_array_contains_distance(key, query, &distance);
        *recheck = true;
        break;
      case ContainedStrategyNumber:
        returnValue = mtree_text_array_overlap_distance(key, query, &distance);
        *recheck = !mtree_text_array_contained_distance(key, query, &distance);
        break;
      default:
        elog(ERROR, "Invalid consistent strategyNumber: %d", strategyNumber);
        break;
    }
  }

  PG_RETURN_BOOL(returnValue);
}

Datum mtree_text_array_union(PG_FUNCTION_ARGS) {
  GistEntryVector* entryVector = (GistEntryVector*) PG_GETARG_POINTER(0);
  GISTENTRY* entry = entryVector->vector;
  int ranges = entryVector->n;

  mtree_text_array* entries[ranges];
  for (int i = 0; i < ranges; ++i) {
    entries[i] = DatumGetMtreeTextArray(entry[i].key);
  }

  int searchRange;

  switch(UNION_STRATEGY_text_array) {
    case First:
      searchRange = 1;
      break;
    case Best:
      searchRange = ranges;
      break;
    default:
      elog(ERROR, "Invalid union strategy: %d", UNION_STRATEGY_text_array);
      break;
  }

  int coveringRadii[searchRange];

  for (int i = 0; i < searchRange; ++i) {
    coveringRadii[i] = 0;

    for (int j = 0; j < ranges; ++j) {
      int distance = mtree_text_array_distance_internal(entries[i], entries[j]);
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

  mtree_text_array* out = mtree_text_array_deep_copy(entries[minimumIndex]);
  out->coveringRadius = coveringRadii[minimumIndex];

  PG_RETURN_MTREE_TEXT_ARRAY_P(out);
}

Datum mtree_text_array_same(PG_FUNCTION_ARGS) {
  mtree_text_array* first = PG_GETARG_MTREE_TEXT_ARRAY_P(0);
  mtree_text_array* second = PG_GETARG_MTREE_TEXT_ARRAY_P(1);
  PG_RETURN_BOOL(mtree_text_array_equals(first, second));
}

Datum mtree_text_array_penalty(PG_FUNCTION_ARGS) {
  GISTENTRY* originalEntry = (GISTENTRY*) PG_GETARG_POINTER(0);
  GISTENTRY* newEntry = (GISTENTRY*) PG_GETARG_POINTER(1);
  float* penalty = (float*) PG_GETARG_POINTER(2);
  mtree_text_array* original = DatumGetMtreeTextArray(originalEntry->key);
  mtree_text_array* new = DatumGetMtreeTextArray(newEntry->key);

  int distance = mtree_text_array_distance_internal(original, new);
  int newCoveringRadius = distance + new->coveringRadius;
  *penalty = (float) (newCoveringRadius < original->coveringRadius ? 0 : newCoveringRadius - original->coveringRadius);

  PG_RETURN_POINTER(penalty);
}

/* TODO: Lots of duplicate code. */
Datum mtree_text_array_picksplit(PG_FUNCTION_ARGS) {
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

  mtree_text_array* entries[maxOffset];
  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    entries[i - FirstOffsetNumber] = DatumGetMtreeTextArray(entryVector->vector[i].key);
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

  switch (PICKSPLIT_STRATEGY_text_array) {
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
        int distance = get_text_array_distance(maxOffset, entries, distances, 0, r);
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
          int distance = get_text_array_distance(maxOffset, entries, distances, l, r);
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
          int leftDistance = get_text_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_text_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
          int leftDistance = get_text_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_text_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
        int distance = get_text_array_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
        int leftRadius = 0, rightRadius = 0;

        for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
          int leftDistance = get_text_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_text_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
          int leftDistance = get_text_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
          int rightDistance = get_text_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
      elog(ERROR, "Invalid picksplit strategy: %d", PICKSPLIT_STRATEGY_text_array);
      break;
  }

  mtree_text_array* unionLeft = mtree_text_array_deep_copy(entries[leftIndex]);
  mtree_text_array* unionRight = mtree_text_array_deep_copy(entries[rightIndex]);
  mtree_text_array* current;

  for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
    int distanceLeft = get_text_array_distance(maxOffset, entries, distances, leftIndex, i - 1);
    int distanceRight = get_text_array_distance(maxOffset, entries, distances, rightIndex, i - 1);
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

Datum mtree_text_array_compress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_text_array_decompress(PG_FUNCTION_ARGS) {
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_text_array_distance(PG_FUNCTION_ARGS) {
  GISTENTRY* entry = (GISTENTRY*) PG_GETARG_POINTER(0);
  mtree_text_array* query = PG_GETARG_MTREE_TEXT_ARRAY_P(1);
  mtree_text_array* key = DatumGetMtreeTextArray(entry->key);
  bool isLeaf = GistPageIsLeaf(entry->page);
  bool *recheck = (bool *) PG_GETARG_POINTER(4);

  if (isLeaf) {
	  *recheck = true;
  }

  PG_RETURN_FLOAT4((float4) mtree_text_array_distance_internal(query, key));
}

Datum mtree_text_array_distance_operator(PG_FUNCTION_ARGS) {
  mtree_text_array* first = PG_GETARG_MTREE_TEXT_ARRAY_P(0);
  mtree_text_array* second = PG_GETARG_MTREE_TEXT_ARRAY_P(1);

  PG_RETURN_FLOAT4((float4) mtree_text_array_distance_internal(first, second));
}

Datum mtree_text_array_overlap_operator(PG_FUNCTION_ARGS) {
  mtree_text_array* first = PG_GETARG_MTREE_TEXT_ARRAY_P(0);
  mtree_text_array* second = PG_GETARG_MTREE_TEXT_ARRAY_P(1);
  bool result = mtree_text_array_overlap_distance(first, second, mtree_text_array_distance_internal(first, second));

  PG_RETURN_BOOL(result);
}

Datum mtree_text_array_contains_operator(PG_FUNCTION_ARGS) {
  mtree_text_array* first = PG_GETARG_MTREE_TEXT_ARRAY_P(0);
  mtree_text_array* second = PG_GETARG_MTREE_TEXT_ARRAY_P(1);
  bool result = mtree_text_array_contains_distance(first, second, mtree_text_array_distance_internal(first, second));

  PG_RETURN_BOOL(result);
}

Datum mtree_text_array_contained_operator(PG_FUNCTION_ARGS) {
  mtree_text_array* first = PG_GETARG_MTREE_TEXT_ARRAY_P(0);
  mtree_text_array* second = PG_GETARG_MTREE_TEXT_ARRAY_P(1);
  bool result = mtree_text_array_contains_distance(second, first, mtree_text_array_distance_internal(second, first));

  PG_RETURN_BOOL(result);
}
