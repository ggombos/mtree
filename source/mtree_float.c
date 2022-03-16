/*
 * contrib/mtree_gist/mtree_float.c
 */

#include "mtree_float.h"

#include "mtree_float_util.h"
#include "mtree_util.h"

 // TODO: Strategy should be a parameter!
const UnionStrategy UNION_STRATEGY_FLOAT = Best;
const PicksplitStrategy PICKSPLIT_STRATEGY_FLOAT = SamplingMinOverlapArea;

PG_FUNCTION_INFO_V1(mtree_float_input);
PG_FUNCTION_INFO_V1(mtree_float_output);

PG_FUNCTION_INFO_V1(mtree_float_consistent);
PG_FUNCTION_INFO_V1(mtree_float_union);
PG_FUNCTION_INFO_V1(mtree_float_same);

PG_FUNCTION_INFO_V1(mtree_float_penalty);
PG_FUNCTION_INFO_V1(mtree_float_picksplit);

PG_FUNCTION_INFO_V1(mtree_float_compress);
PG_FUNCTION_INFO_V1(mtree_float_decompress);

PG_FUNCTION_INFO_V1(mtree_float_distance);

PG_FUNCTION_INFO_V1(mtree_float_contains_operator);
PG_FUNCTION_INFO_V1(mtree_float_contained_operator);
PG_FUNCTION_INFO_V1(mtree_float_distance_operator);
PG_FUNCTION_INFO_V1(mtree_float_overlap_operator);

Datum mtree_float_input(PG_FUNCTION_ARGS) {
	char* input = PG_GETARG_CSTRING(0);

	elog(INFO, "input [char*]: %s", input);

	mtree_float* result = (mtree_float*)palloc(MTREE_FLOAT_SIZE);
	result->coveringRadius = 0;
	result->parentDistance = 0;

	SET_VARSIZE(result, MTREE_FLOAT_SIZE);

	char* tmp;
	result->data = atof(input);

	elog(INFO, "input [float]: %f", result->data);

	PG_RETURN_POINTER(result);
}

Datum mtree_float_output(PG_FUNCTION_ARGS) {
	mtree_float* output = PG_GETARG_MTREE_FLOAT_P(0);
	char* result;

	elog(INFO, "output [float]: %f", output->data);

	if (output->coveringRadius == 0) {
		result = psprintf("%f", output->data);
	}
	else {
		result =
			psprintf("coveringRadius|%d parentDistance|%d data|%f",
				output->coveringRadius, output->parentDistance, output->data);
	}

	elog(INFO, "output [char*]: %s", result);

	PG_RETURN_CSTRING(result);
}

Datum mtree_float_consistent(PG_FUNCTION_ARGS) {
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float* query = PG_GETARG_MTREE_FLOAT_P(1);
	StrategyNumber strategyNumber = (StrategyNumber)PG_GETARG_UINT16(2);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);
	mtree_float* key = DatumGetMtreeFloat(entry->key);
	float distance = mtree_float_distance_internal(key, query);

	bool returnValue;
	if (GIST_LEAF(entry)) {
		*recheck = false;
		switch (strategyNumber) {
		case SameStrategyNumber:
			returnValue = mtree_float_equals(key, query);
			break;
		case OverlapStrategyNumber:
			returnValue = mtree_float_overlap_distance(key, query, distance);
			break;
		case ContainsStrategyNumber:
			returnValue = mtree_float_contains_distance(key, query, distance);
			break;
		case ContainedStrategyNumber:
			returnValue = mtree_float_contained_distance(key, query, distance);
			break;
		default:
			elog(ERROR, "Invalid consistent strategyNumber: %d", strategyNumber);
			break;
		}
	}
	else {
		switch (strategyNumber) {
		case SameStrategyNumber:
			returnValue = mtree_float_contains_distance(key, query, distance);
			*recheck = true;
			break;
		case OverlapStrategyNumber:
			returnValue = mtree_float_overlap_distance(key, query, distance);
			*recheck = !mtree_float_contained_distance(key, query, distance);
			break;
		case ContainsStrategyNumber:
			returnValue = mtree_float_contains_distance(key, query, distance);
			*recheck = true;
			break;
		case ContainedStrategyNumber:
			returnValue = mtree_float_overlap_distance(key, query, distance);
			*recheck = !mtree_float_contained_distance(key, query, distance);
			break;
		default:
			elog(ERROR, "Invalid consistent strategyNumber: %d", strategyNumber);
			break;
		}
	}

	PG_RETURN_BOOL(returnValue);
}

Datum mtree_float_union(PG_FUNCTION_ARGS) {
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GISTENTRY* entry = entryVector->vector;
	int ranges = entryVector->n;

	mtree_float* entries[ranges];
	for (int i = 0; i < ranges; ++i) {
		entries[i] = DatumGetMtreeFloat(entry[i].key);
	}

	int searchRange;

	switch (UNION_STRATEGY_FLOAT) {
	case First:
		searchRange = 1;
		break;
	case Best:
		searchRange = ranges;
		break;
	default:
		elog(ERROR, "Invalid union strategy: %d", UNION_STRATEGY_FLOAT);
		break;
	}

	int coveringRadii[searchRange];

	for (int i = 0; i < searchRange; ++i) {
		coveringRadii[i] = 0;

		for (int j = 0; j < ranges; ++j) {
			int distance = mtree_float_distance_internal(entries[i], entries[j]);
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

	mtree_float* out = mtree_float_deep_copy(entries[minimumIndex]);
	out->coveringRadius = coveringRadii[minimumIndex];

	PG_RETURN_MTREE_FLOAT_P(out);
}

Datum mtree_float_same(PG_FUNCTION_ARGS) {
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	PG_RETURN_BOOL(mtree_float_equals(first, second));
}

Datum mtree_float_penalty(PG_FUNCTION_ARGS) {
	GISTENTRY* originalEntry = (GISTENTRY*)PG_GETARG_POINTER(0);
	GISTENTRY* newEntry = (GISTENTRY*)PG_GETARG_POINTER(1);
	float* penalty = (float*)PG_GETARG_POINTER(2);
	mtree_float* original = DatumGetMtreeFloat(originalEntry->key);
	mtree_float* new = DatumGetMtreeFloat(newEntry->key);

	int distance = mtree_float_distance_internal(original, new);
	int newCoveringRadius = distance + new->coveringRadius;
	*penalty = (float)(newCoveringRadius < original->coveringRadius
		? 0
		: newCoveringRadius - original->coveringRadius);

	PG_RETURN_POINTER(penalty);
}

/* TODO: Lots of duplicate code. */
Datum mtree_float_picksplit(PG_FUNCTION_ARGS) {
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GIST_SPLITVEC* vector = (GIST_SPLITVEC*)PG_GETARG_POINTER(1);
	OffsetNumber maxOffset = (OffsetNumber)entryVector->n - 1;
	OffsetNumber numberBytes =
		(OffsetNumber)(maxOffset + 1) * sizeof(OffsetNumber);
	OffsetNumber* left;
	OffsetNumber* right;

	vector->spl_left = (OffsetNumber*)palloc(numberBytes);
	left = vector->spl_left;
	vector->spl_nleft = 0;

	vector->spl_right = (OffsetNumber*)palloc(numberBytes);
	right = vector->spl_right;
	vector->spl_nright = 0;

	mtree_float* entries[maxOffset];
	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset;
		i = OffsetNumberNext(i)) {
		entries[i - FirstOffsetNumber] =
			DatumGetMtreeFloat(entryVector->vector[i].key);
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

	switch (PICKSPLIT_STRATEGY_FLOAT) {
	case Random:
		leftIndex = ((int)random()) % (maxOffset - 1);
		rightIndex =
			(leftIndex + 1) + (((int)random()) % (maxOffset - leftIndex - 1));
		break;
	case FirstTwo:
		leftIndex = 0;
		rightIndex = 1;
		break;
	case MaxDistanceFromFirst:
		maxDistance = -1;
		for (int r = 0; r < maxOffset; ++r) {
			int distance = get_float_distance(maxOffset, entries, distances, 0, r);
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
				int distance = get_float_distance(maxOffset, entries, distances, l, r);
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
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex =
				(leftCandidateIndex + 1) +
				(((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
				int leftDistance = get_float_distance(maxOffset, entries, distances,
					leftCandidateIndex, currentIndex);
				int rightDistance = get_float_distance(
					maxOffset, entries, distances, rightCandidateIndex, currentIndex);

				if (leftDistance < rightDistance) {
					if (leftDistance + entries[currentIndex]->coveringRadius >
						leftRadius) {
						leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
					}
				}
				else {
					if (rightDistance + entries[currentIndex]->coveringRadius >
						rightRadius) {
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
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex =
				(leftCandidateIndex + 1) +
				(((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; ++currentIndex) {
				int leftDistance = get_float_distance(maxOffset, entries, distances,
					leftCandidateIndex, currentIndex);
				int rightDistance = get_float_distance(
					maxOffset, entries, distances, rightCandidateIndex, currentIndex);

				if (leftDistance < rightDistance) {
					if (leftDistance + entries[currentIndex]->coveringRadius >
						leftRadius) {
						leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
					}
				}
				else {
					if (rightDistance + entries[currentIndex]->coveringRadius >
						rightRadius) {
						rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
					}
				}
			}

			if (minCoveringMax == -1 ||
				MAX2(leftRadius, rightRadius) < minCoveringMax) {
				minCoveringMax = MAX2(leftRadius, rightRadius);
				leftIndex = leftCandidateIndex;
				rightIndex = rightCandidateIndex;
			}
		}
		break;
	case SamplingMinOverlapArea:
		for (int i = 0; i < trialCount; i++) {
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex =
				(leftCandidateIndex + 1) +
				(((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int distance = get_float_distance(maxOffset, entries, distances,
				leftCandidateIndex, rightCandidateIndex);
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
				int leftDistance = get_float_distance(maxOffset, entries, distances,
					leftCandidateIndex, currentIndex);
				int rightDistance = get_float_distance(
					maxOffset, entries, distances, rightCandidateIndex, currentIndex);

				if (leftDistance < rightDistance) {
					if (leftDistance + entries[currentIndex]->coveringRadius >
						leftRadius) {
						leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
					}
				}
				else {
					if (rightDistance + entries[currentIndex]->coveringRadius >
						rightRadius) {
						rightRadius = rightDistance + entries[currentIndex]->coveringRadius;
					}
				}
			}

			double currentOverlapArea =
				overlap_area(leftRadius, rightRadius, distance);
			if (minOverlapArea == -1 || currentOverlapArea < minOverlapArea) {
				minOverlapArea = (int)currentOverlapArea;
				leftIndex = leftCandidateIndex;
				rightIndex = rightCandidateIndex;
			}
		}
		break;
	case SamplingMinAreaSum:
		for (int i = 0; i < trialCount; i++) {
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex =
				(leftCandidateIndex + 1) +
				(((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
				int leftDistance = get_float_distance(maxOffset, entries, distances,
					leftCandidateIndex, currentIndex);
				int rightDistance = get_float_distance(
					maxOffset, entries, distances, rightCandidateIndex, currentIndex);

				if (leftDistance < rightDistance) {
					if (leftDistance + entries[currentIndex]->coveringRadius >
						leftRadius) {
						leftRadius = leftDistance + entries[currentIndex]->coveringRadius;
					}
				}
				else {
					if (rightDistance + entries[currentIndex]->coveringRadius >
						rightRadius) {
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
		elog(ERROR, "Invalid picksplit strategy: %d", PICKSPLIT_STRATEGY_FLOAT);
		break;
	}

	mtree_float* unionLeft = mtree_float_deep_copy(entries[leftIndex]);
	mtree_float* unionRight = mtree_float_deep_copy(entries[rightIndex]);
	mtree_float* current;

	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset;
		i = OffsetNumberNext(i)) {
		int distanceLeft =
			get_float_distance(maxOffset, entries, distances, leftIndex, i - 1);
		int distanceRight =
			get_float_distance(maxOffset, entries, distances, rightIndex, i - 1);
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
			if (distanceRight + current->coveringRadius >
				unionRight->coveringRadius) {
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

Datum mtree_float_compress(PG_FUNCTION_ARGS) {
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_decompress(PG_FUNCTION_ARGS) {
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_distance(PG_FUNCTION_ARGS) {
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float* query = PG_GETARG_MTREE_FLOAT_P(1);
	mtree_float* key = DatumGetMtreeFloat(entry->key);
	bool isLeaf = GistPageIsLeaf(entry->page);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);

	if (isLeaf) {
		*recheck = true;
	}

	PG_RETURN_FLOAT4((float4)mtree_float_distance_internal(query, key));
}

Datum mtree_float_distance_operator(PG_FUNCTION_ARGS) {
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);

	PG_RETURN_FLOAT4((float4)mtree_float_distance_internal(first, second));
}

Datum mtree_float_overlap_operator(PG_FUNCTION_ARGS) {
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	bool result = mtree_float_contains_distance(first, second, mtree_float_distance_internal(first, second));

	PG_RETURN_BOOL(result);
}

Datum mtree_float_contains_operator(PG_FUNCTION_ARGS) {
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	bool result = mtree_float_contains_distance(first, second, mtree_float_distance_internal(first, second));

	PG_RETURN_BOOL(result);
}

Datum mtree_float_contained_operator(PG_FUNCTION_ARGS) {
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	bool result = mtree_float_contains_distance(second, first, mtree_float_distance_internal(second, first));

	PG_RETURN_BOOL(result);
}
