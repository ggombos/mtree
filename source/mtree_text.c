/*
 * contrib/mtree_gist/mtree_text.c
 */

#include "mtree_text.h"

#include "mtree_text_util.h"
#include "mtree_util.h"

/*
 * Data type related functions (I/O)
 */

PG_FUNCTION_INFO_V1(mtree_text_input);
PG_FUNCTION_INFO_V1(mtree_text_output);

PG_FUNCTION_INFO_V1(mtree_text_consistent);
PG_FUNCTION_INFO_V1(mtree_text_union);
PG_FUNCTION_INFO_V1(mtree_text_same);

PG_FUNCTION_INFO_V1(mtree_text_penalty);
PG_FUNCTION_INFO_V1(mtree_text_picksplit);

PG_FUNCTION_INFO_V1(mtree_text_distance);

PG_FUNCTION_INFO_V1(mtree_text_overlap_operator);
PG_FUNCTION_INFO_V1(mtree_text_operator_contains);
PG_FUNCTION_INFO_V1(mtree_text_operator_contained);
PG_FUNCTION_INFO_V1(mtree_text_operator_distance);
PG_FUNCTION_INFO_V1(mtree_text_operator_same);

Datum mtree_text_input(PG_FUNCTION_ARGS)
{
	char* input = PG_GETARG_CSTRING(0);

	size_t stringLength = strlen(input);
	mtree_text* result = (mtree_text*)palloc(MTREE_TEXT_SIZE + stringLength * sizeof(char) + 1);
	result->coveringRadius = 0;
	result->parentDistance = 0;

	SET_VARSIZE(result, MTREE_TEXT_SIZE + stringLength * sizeof(char) + 1);

	strcpy(result->vl_data, input);
	result->vl_data[stringLength] = '\0';

	PG_RETURN_POINTER(result);
}

Datum mtree_text_output(PG_FUNCTION_ARGS)
{
	mtree_text* text = PG_GETARG_MTREE_TEXT_P(0);
	char* result;

	if (text->coveringRadius == 0) {
		result = psprintf("%s", text->vl_data);
	} else {
		result = psprintf("distance|%f data|%s", text->coveringRadius, text->vl_data);
	}

	PG_RETURN_CSTRING(result);
}

Datum mtree_text_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_text* query = PG_GETARG_MTREE_TEXT_P(1);
	StrategyNumber strategyNumber = (StrategyNumber)PG_GETARG_UINT16(2);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);
	mtree_text* key = DatumGetMtreeText(entry->key);

	*recheck = false;

	bool returnValue;
	if (GIST_LEAF(entry)) {
		switch (strategyNumber) {
			case GIST_SN_SAME:
				returnValue = mtree_text_equals(key, query);
				break;
			case GIST_SN_OVERLAPS:
				returnValue = mtree_text_overlap_distance(key, query);
				break;
			case GIST_SN_CONTAINS:
				returnValue = mtree_text_contains_distance(key, query);
				break;
			case GIST_SN_CONTAINED_BY:
				returnValue = mtree_text_contained_distance(key, query);
				break;
			default:
				ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
				break;
		}
	} else {
		switch (strategyNumber) {
			case GIST_SN_SAME:
				returnValue = mtree_text_contains_distance(key, query);
				break;
			case GIST_SN_OVERLAPS:
				returnValue = mtree_text_overlap_distance(key, query);
				break;
			case GIST_SN_CONTAINS:
				returnValue = mtree_text_contains_distance(key, query);
				break;
			case GIST_SN_CONTAINED_BY:
				returnValue = mtree_text_overlap_distance(key, query);
				break;
			default:
				ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
				break;
		}
	}

	PG_RETURN_BOOL(returnValue);
}

Datum mtree_text_union(PG_FUNCTION_ARGS)
{
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GISTENTRY* entry = entryVector->vector;
	int ranges = entryVector->n;

	mtree_text* entries[ranges];
	for (int i = 0; i < ranges; ++i) {
		entries[i] = DatumGetMtreeText(entry[i].key);
	}

	/*int searchRange;
	MtreeUnionStrategy union_strategy = MinMaxDistance;
	if (PG_HAS_OPCLASS_OPTIONS()) {
			MtreeOptions* options = (MtreeOptions*)PG_GET_OPCLASS_OPTIONS();
			union_strategy = options->union_strategy;
	}

	switch (union_strategy)
	{
	case First:
			searchRange = 1;
			break;
	case MinMaxDistance:
			searchRange = ranges;
			break;
	default:
			ereport(ERROR,
					errcode(ERRCODE_SYNTAX_ERROR),
					errmsg("Invalid strategy for mtree_text_union: %hu",
	union_strategy)); break;
	}

	int coveringRadii[searchRange];

	for (int i = 0; i < searchRange; ++i)
	{
			coveringRadii[i] = 0;

			for (int j = 0; j < ranges; ++j)
			{
					int distance = mtree_text_outer_distance(entries[i],
	entries[j]); int newCoveringRadius = distance + entries[j]->coveringRadius;

					if (coveringRadii[i] < newCoveringRadius)
					{
							coveringRadii[i] = newCoveringRadius;
					}
			}
	}

	int minimumIndex = 0;

	for (int i = 1; i < searchRange; ++i)
	{
			if (coveringRadii[i] < coveringRadii[minimumIndex])
			{
					minimumIndex = i;
			}
	}*/

	// mtree_text* out = mtree_text_deep_copy(entries[minimumIndex]);
	// out->coveringRadius = coveringRadii[minimumIndex];

	mtree_text* out = mtree_text_deep_copy(entries[0]);
	out->coveringRadius += mtree_text_outer_distance(entries[0], entries[1]);

	PG_RETURN_MTREE_TEXT_P(out);
}

Datum mtree_text_same(PG_FUNCTION_ARGS)
{
	mtree_text* first = (mtree_text*)PG_GETARG_POINTER(0);
	mtree_text* second = (mtree_text*)PG_GETARG_POINTER(1);
	PG_RETURN_BOOL(mtree_text_equals(first, second));
}

Datum mtree_text_penalty(PG_FUNCTION_ARGS)
{
	GISTENTRY* originalEntry = (GISTENTRY*)PG_GETARG_POINTER(0);
	GISTENTRY* newEntry = (GISTENTRY*)PG_GETARG_POINTER(1);
	float* penalty = (float*)PG_GETARG_POINTER(2);
	mtree_text* original = DatumGetMtreeText(originalEntry->key);
	mtree_text* new = DatumGetMtreeText(newEntry->key);

	double distance = mtree_text_outer_distance(original, new);
	*penalty = distance;

	PG_RETURN_POINTER(penalty);
}

Datum mtree_text_picksplit(PG_FUNCTION_ARGS)
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

	mtree_text* entries[maxOffset];
	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		entries[i - FirstOffsetNumber] = DatumGetMtreeText(entryVector->vector[i].key);
	}

	double distances[maxOffset][maxOffset];
	init_distances(maxOffset, *distances);

	int leftIndex, rightIndex, leftCandidateIndex, rightCandidateIndex;
	int trialCount = 100;
	double maxDistance = -1.0;
	double minCoveringSum = -1.0;
	double minCoveringMax = -1.0;
	double minOverlapArea = -1.0;
	double minSumArea = -1.0;

	MtreePickSplitStrategy picksplit_strategy = SamplingMinOverlapArea;
	if (PG_HAS_OPCLASS_OPTIONS()) {
		MtreeOptions* options = (MtreeOptions*)PG_GET_OPCLASS_OPTIONS();
		picksplit_strategy = options->picksplit_strategy;
	}
	switch (picksplit_strategy) {
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
				double distance = get_distance(maxOffset, entries, distances, 0, r);
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
					double distance = get_distance(maxOffset, entries, distances, l, r);
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
					double distanceLeft = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
					double distanceLeft = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
					double distance = get_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
					double leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					double distanceLeft = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
					minOverlapArea = currentOverlapArea;
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
					double distanceLeft = get_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					double distanceRight = get_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
					errmsg("Invalid strategy for mtree_text_picksplit: %hu", picksplit_strategy));
			break;
	}

	mtree_text* unionLeft = mtree_text_deep_copy(entries[leftIndex]);
	mtree_text* unionRight = mtree_text_deep_copy(entries[rightIndex]);
	mtree_text* current;

	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		double distanceLeft = get_distance(maxOffset, entries, distances, leftIndex, i - 1);
		double distanceRight = get_distance(maxOffset, entries, distances, rightIndex, i - 1);
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

Datum mtree_text_distance(PG_FUNCTION_ARGS)
{
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_text* query = PG_GETARG_MTREE_TEXT_P(1);
	mtree_text* key = DatumGetMtreeText(entry->key);

	PG_RETURN_FLOAT8((float8)mtree_text_outer_distance(query, key));
}

Datum mtree_text_operator_distance(PG_FUNCTION_ARGS)
{
	mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
	mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);

	PG_RETURN_FLOAT8((float8)mtree_text_outer_distance(first, second));
}

Datum mtree_text_overlap_operator(PG_FUNCTION_ARGS)
{
	mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
	mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);

	bool result = mtree_text_overlap_wrapper(first, second);

	PG_RETURN_BOOL(result);
}

Datum mtree_text_operator_same(PG_FUNCTION_ARGS)
{
	mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
	mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);

	bool result = mtree_text_equals(first, second);

	PG_RETURN_BOOL(result);
}

Datum mtree_text_operator_contains(PG_FUNCTION_ARGS)
{
	mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
	mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);
	PG_RETURN_BOOL(mtree_text_contains_wrapper(first, second));
}

Datum mtree_text_operator_contained(PG_FUNCTION_ARGS)
{
	mtree_text* first = PG_GETARG_MTREE_TEXT_P(0);
	mtree_text* second = PG_GETARG_MTREE_TEXT_P(1);
	PG_RETURN_BOOL(mtree_text_contained_wrapper(first, second));
}
