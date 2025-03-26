/*
 * contrib/mtree_gist/mtree_float.c
 */

#include "mtree_float.h"

#include "mtree_float_util.h"
#include "mtree_util.h"

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

Datum mtree_float_input(PG_FUNCTION_ARGS)
{
	char* input = PG_GETARG_CSTRING(0);

	mtree_float* result = (mtree_float*)palloc(MTREE_FLOAT_SIZE);
	result->coveringRadius = 0.0;
	result->parentDistance = 0.0;

	SET_VARSIZE(result, MTREE_FLOAT_SIZE);

	// char* tmp;
	result->data = atof(input);

	PG_RETURN_POINTER(result);
}

Datum mtree_float_output(PG_FUNCTION_ARGS)
{
	mtree_float* output = PG_GETARG_MTREE_FLOAT_P(0);
	char* result;

	if (output->coveringRadius == 0) {
		result = psprintf("%f", output->data);
	} else {
		result = psprintf("coveringRadius|%f parentDistance|%f data|%f", output->coveringRadius, output->parentDistance,
						  output->data);
	}

	PG_RETURN_CSTRING(result);
}

Datum mtree_float_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float* query = PG_GETARG_MTREE_FLOAT_P(1);
	StrategyNumber strategyNumber = (StrategyNumber)PG_GETARG_UINT16(2);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);
	mtree_float* key = DatumGetMtreeFloat(entry->key);

	*recheck = false;

	bool returnValue;
	if (GIST_LEAF(entry)) {
		switch (strategyNumber) {
			case GIST_SN_SAME:
				returnValue = mtree_float_equals(key, query);
				break;
			case GIST_SN_OVERLAPS:
				returnValue = mtree_float_overlap_distance(key, query);
				break;
			case GIST_SN_CONTAINS:
				returnValue = mtree_float_contains_distance(key, query);
				break;
			case GIST_SN_CONTAINED_BY:
				returnValue = mtree_float_contained_distance(key, query);
				break;
			default:
				ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
				break;
		}
	} else {
		switch (strategyNumber) {
			case GIST_SN_SAME:
				returnValue = mtree_float_contains_distance(key, query);
				break;
			case GIST_SN_OVERLAPS:
				returnValue = mtree_float_overlap_distance(key, query);
				break;
			case GIST_SN_CONTAINS:
				returnValue = mtree_float_contains_distance(key, query);
				break;
			case GIST_SN_CONTAINED_BY:
				returnValue = mtree_float_overlap_distance(key, query);
				break;
			default:
				ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
				break;
		}
	}

	PG_RETURN_BOOL(returnValue);
}

Datum mtree_float_union(PG_FUNCTION_ARGS)
{
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GISTENTRY* entry = entryVector->vector;
	int ranges = entryVector->n;

	mtree_float* entries[ranges];
	for (int i = 0; i < ranges; ++i) {
		entries[i] = DatumGetMtreeFloat(entry[i].key);
	}

	/*int searchRange;

	MtreeUnionStrategy UNION_STRATEGY_FLOAT = MinMaxDistance;
	if (PG_HAS_OPCLASS_OPTIONS())
	{
		MtreeOptions* options = (MtreeOptions *) PG_GET_OPCLASS_OPTIONS();
		UNION_STRATEGY_FLOAT = options->union_strategy;
	}

	switch (UNION_STRATEGY_FLOAT) {
	case First:
		searchRange = 1;
		break;
	case MinMaxDistance:
		searchRange = ranges;
		break;
	default:
		ereport(ERROR,
			errcode(ERRCODE_SYNTAX_ERROR),
			errmsg("Invalid StrategyNumber for union function: %u", UNION_STRATEGY_FLOAT));
		break;
	}

	double coveringRadii[searchRange];

	for (int i = 0; i < searchRange; ++i) {
		coveringRadii[i] = 0;

		for (int j = 0; j < ranges; ++j) {
			double distance = mtree_float_outer_distance(entries[i], entries[j]);
			//elog(INFO, "mtree_float_union: distance: %f", distance);
			double newCoveringRadius = distance + entries[j]->coveringRadius;
			//elog(INFO, "mtree_float_union: newCoveringRadius: %f", newCoveringRadius);

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
	}*/

	// mtree_float* out = mtree_float_deep_copy(entries[minimumIndex]);
	// out->coveringRadius = coveringRadii[minimumIndex];

	mtree_float* out = mtree_float_deep_copy(entries[0]);
	out->coveringRadius += mtree_float_outer_distance(entries[0], entries[1]);

	PG_RETURN_MTREE_FLOAT_P(out);
}

Datum mtree_float_same(PG_FUNCTION_ARGS)
{
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	PG_RETURN_BOOL(mtree_float_equals(first, second));
}

Datum mtree_float_penalty(PG_FUNCTION_ARGS)
{
	GISTENTRY* originalEntry = (GISTENTRY*)PG_GETARG_POINTER(0);
	GISTENTRY* newEntry = (GISTENTRY*)PG_GETARG_POINTER(1);
	float* penalty = (float*)PG_GETARG_POINTER(2);
	mtree_float* original = DatumGetMtreeFloat(originalEntry->key);
	mtree_float* new = DatumGetMtreeFloat(newEntry->key);

	double distance = mtree_float_outer_distance(original, new);
	*penalty = distance;
	PG_RETURN_POINTER(penalty);
}

/* TODO: Lots of duplicate code. */
Datum mtree_float_picksplit(PG_FUNCTION_ARGS)
{
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GIST_SPLITVEC* vector = (GIST_SPLITVEC*)PG_GETARG_POINTER(1);
	OffsetNumber maxOffset = (OffsetNumber)entryVector->n - 1;
	OffsetNumber numberBytes = (OffsetNumber)(maxOffset + 1) * sizeof(OffsetNumber);
	OffsetNumber* left;
	OffsetNumber* right;

	vector->spl_left = (OffsetNumber*)palloc(numberBytes);
	left = vector->spl_left;
	vector->spl_nleft = 0;

	vector->spl_right = (OffsetNumber*)palloc(numberBytes);
	right = vector->spl_right;
	vector->spl_nright = 0;

	mtree_float* entries[maxOffset];
	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		entries[i - FirstOffsetNumber] = DatumGetMtreeFloat(entryVector->vector[i].key);
	}

	double distances[maxOffset][maxOffset];
	init_distances(maxOffset, *distances);

	int leftIndex, rightIndex, leftCandidateIndex, rightCandidateIndex;
	int trialCount = 100;
	double maxDistance = -1;
	double minCoveringSum = -1.0;
	double minCoveringMax = -1.0;
	double minOverlapArea = -1.0;
	double minSumArea = -1.0;

	MtreePickSplitStrategy PICKSPLIT_STRATEGY_FLOAT = SamplingMinOverlapArea;
	if (PG_HAS_OPCLASS_OPTIONS()) {
		MtreeOptions* options = (MtreeOptions*)PG_GET_OPCLASS_OPTIONS();
		PICKSPLIT_STRATEGY_FLOAT = options->picksplit_strategy;
	}

	switch (PICKSPLIT_STRATEGY_FLOAT) {
		case Random:
			leftIndex = ((int)random()) % (maxOffset - 1);
			rightIndex = (leftIndex + 1) + (((int)random()) % (maxOffset - leftIndex - 1));
			break;
		case FirstTwo:
			leftIndex = -1;
			rightIndex = -1;

			for (int i = 0; i < maxOffset - 1; ++i) {
				if (entries[i]->level == entries[i + 1]->level) {
					leftIndex = i;
					rightIndex = i + 1;
					break;
				}
			}

			break;
		case MaxDistanceFromFirst:
			maxDistance = -1.0;
			for (int r = 0; r < maxOffset; ++r) {
				double distance = get_float_distance(maxOffset, entries, distances, 0, r);
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
					double distance = get_float_distance(maxOffset, entries, distances, l, r);
					if (distance > maxDistance) {
						maxDistance = distance;
						leftCandidateIndex = l;
						rightCandidateIndex = r;
					}
				}
			}
			leftIndex = leftCandidateIndex;
			rightIndex = rightCandidateIndex;
			break;
		case SamplingMinCoveringSum:
			for (int i = 0; i < trialCount; ++i) {
				leftCandidateIndex = ((int)random()) % (maxOffset - 1);
				rightCandidateIndex =
					(leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
				double leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					double distanceLeft =
						get_float_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight =
						get_float_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

					if (distanceLeft < distanceRight) {
						if (distanceLeft + entries[currentIndex]->coveringRadius > leftRadius) {
							leftRadius = distanceLeft + entries[currentIndex]->coveringRadius;
						}
					} else {
						if (distanceRight + entries[currentIndex]->coveringRadius > rightRadius) {
							rightRadius = distanceRight + entries[currentIndex]->coveringRadius;
						}
					}
				}

				if (minCoveringSum == -1.0 || leftRadius + rightRadius < minCoveringSum) {
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
					(leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
				double leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; ++currentIndex) {
					double distanceLeft =
						get_float_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight =
						get_float_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

					if (distanceLeft < distanceRight) {
						if (distanceLeft + entries[currentIndex]->coveringRadius > leftRadius) {
							leftRadius = distanceLeft + entries[currentIndex]->coveringRadius;
						}
					} else {
						if (distanceRight + entries[currentIndex]->coveringRadius > rightRadius) {
							rightRadius = distanceRight + entries[currentIndex]->coveringRadius;
						}
					}
				}

				if (minCoveringMax == -1.0 || MAX_2(leftRadius, rightRadius) < minCoveringMax) {
					minCoveringMax = MAX_2(leftRadius, rightRadius);
					leftIndex = leftCandidateIndex;
					rightIndex = rightCandidateIndex;
				}
			}
			break;
		case SamplingMinOverlapArea:
			for (int i = 0; i < trialCount; i++) {
				leftCandidateIndex = ((int)random()) % (maxOffset - 1);
				rightCandidateIndex =
					(leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
				double distance =
					get_float_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
				double leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					double distanceLeft =
						get_float_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight =
						get_float_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

					if (distanceLeft < distanceRight) {
						if (distanceLeft + entries[currentIndex]->coveringRadius > leftRadius) {
							leftRadius = distanceLeft + entries[currentIndex]->coveringRadius;
						}
					} else {
						if (distanceRight + entries[currentIndex]->coveringRadius > rightRadius) {
							rightRadius = distanceRight + entries[currentIndex]->coveringRadius;
						}
					}
				}

				double currentOverlapArea = overlap_area(leftRadius, rightRadius, distance);
				if (minOverlapArea == -1.0 || currentOverlapArea < minOverlapArea) {
					minOverlapArea = (float)currentOverlapArea;
					leftIndex = leftCandidateIndex;
					rightIndex = rightCandidateIndex;
				}
			}
			break;
		case SamplingMinAreaSum:
			for (int i = 0; i < trialCount; i++) {
				leftCandidateIndex = ((int)random()) % (maxOffset - 1);
				rightCandidateIndex =
					(leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
				double leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					double distanceLeft =
						get_float_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight =
						get_float_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

					if (distanceLeft < distanceRight) {
						if (distanceLeft + entries[currentIndex]->coveringRadius > leftRadius) {
							leftRadius = distanceLeft + entries[currentIndex]->coveringRadius;
						}
					} else {
						if (distanceRight + entries[currentIndex]->coveringRadius > rightRadius) {
							rightRadius = distanceRight + entries[currentIndex]->coveringRadius;
						}
					}
				}

				double currentSumArea = leftRadius * leftRadius + rightRadius * rightRadius;
				if (minSumArea == -1.0 || currentSumArea < minSumArea) {
					minSumArea = currentSumArea;
					leftIndex = leftCandidateIndex;
					rightIndex = rightCandidateIndex;
				}
			}
			break;
		default:
			ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
					errmsg("Invalid StrategyNumber for picksplit function: %u", PICKSPLIT_STRATEGY_FLOAT));
			break;
	}

	mtree_float* unionLeft = mtree_float_deep_copy(entries[leftIndex]);
	mtree_float* unionRight = mtree_float_deep_copy(entries[rightIndex]);
	mtree_float* current;

	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		double distanceLeft = get_float_distance(maxOffset, entries, distances, leftIndex, i - 1);
		double distanceRight = get_float_distance(maxOffset, entries, distances, rightIndex, i - 1);
		current = entries[i - 1];

		if (distanceLeft < distanceRight) {
			if (distanceLeft + current->coveringRadius > unionLeft->coveringRadius) {
				unionLeft->coveringRadius = distanceLeft + current->coveringRadius;
			}
			*left = i;
			++left;
			++(vector->spl_nleft);
		} else {
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

Datum mtree_float_compress(PG_FUNCTION_ARGS)
{
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_decompress(PG_FUNCTION_ARGS)
{
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_distance(PG_FUNCTION_ARGS)
{
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float* query = PG_GETARG_MTREE_FLOAT_P(1);
	mtree_float* key = DatumGetMtreeFloat(entry->key);

	PG_RETURN_FLOAT8((float8)mtree_float_outer_distance(query, key));
}

Datum mtree_float_distance_operator(PG_FUNCTION_ARGS)
{
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);

	PG_RETURN_FLOAT8((float8)mtree_float_outer_distance(first, second));
}

Datum mtree_float_overlap_operator(PG_FUNCTION_ARGS)
{
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	bool result = mtree_float_overlap_distance(first, second);

	PG_RETURN_BOOL(result);
}

Datum mtree_float_contains_operator(PG_FUNCTION_ARGS)
{
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	bool result = mtree_float_contains_distance(first, second);

	PG_RETURN_BOOL(result);
}

Datum mtree_float_contained_operator(PG_FUNCTION_ARGS)
{
	mtree_float* first = PG_GETARG_MTREE_FLOAT_P(0);
	mtree_float* second = PG_GETARG_MTREE_FLOAT_P(1);
	bool result = mtree_float_contains_distance(second, first);

	PG_RETURN_BOOL(result);
}
