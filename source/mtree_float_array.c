/*
 * contrib/mtree_gist/mtree_float_array.c
 */

#include "mtree_float_array.h"
#include "mtree_float_array_util.h"

#include "mtree_util.h"

 /* TODO: Strategy should be a parameter! */
const MtreeUnionStrategy UNION_STRATEGY_FLOAT_ARRAY = MinMaxDistance;
const MtreePickSplitStrategy PICKSPLIT_STRATEGY_FLOAT_ARRAY = SamplingMinOverlapArea;

PG_FUNCTION_INFO_V1(mtree_float_array_input);
PG_FUNCTION_INFO_V1(mtree_float_array_output);

PG_FUNCTION_INFO_V1(mtree_float_array_consistent);
PG_FUNCTION_INFO_V1(mtree_float_array_union);
PG_FUNCTION_INFO_V1(mtree_float_array_same);

PG_FUNCTION_INFO_V1(mtree_float_array_penalty);
PG_FUNCTION_INFO_V1(mtree_float_array_picksplit);

PG_FUNCTION_INFO_V1(mtree_float_array_compress);
PG_FUNCTION_INFO_V1(mtree_float_array_decompress);

PG_FUNCTION_INFO_V1(mtree_float_array_distance);

PG_FUNCTION_INFO_V1(mtree_float_array_contains_operator);
PG_FUNCTION_INFO_V1(mtree_float_array_contained_operator);
PG_FUNCTION_INFO_V1(mtree_float_array_distance_operator);
PG_FUNCTION_INFO_V1(mtree_float_array_overlap_operator);

Datum mtree_float_array_input(PG_FUNCTION_ARGS) {
	char* input = PG_GETARG_CSTRING(0);
	unsigned char inputLength = strlen(input);

	if (inputLength == 0) {
		ereport(ERROR,
			errcode(ERRCODE_SYNTAX_ERROR),
			errmsg("The input is an empty string."));
	}

	char previousInteger = '\0';
	unsigned char arrayLength = 1;
	for (unsigned char i = 0; i < inputLength; ++i) {
		if (isblank(input[i])) {
			ereport(ERROR,
				errcode(ERRCODE_SYNTAX_ERROR),
				errmsg("The array can not contain space or tab characters."));
		}
		else if (input[i] == ',' && previousInteger != '\0') {
			++arrayLength;
			previousInteger = '\0';
		}
		else if (!isdigit(input[i]) && input[i] != '-' && input[i] != '.') {
			ereport(ERROR,
				errcode(ERRCODE_SYNTAX_ERROR),
				errmsg("The array can only contain integers [0-9] and commas [,]."));
		}
		previousInteger = input[i];
	}

	size_t size = MTREE_FLOAT_ARRAY_SIZE + arrayLength * sizeof(int64) + 1;
	mtree_float_array* result = (mtree_float_array*)palloc(size);

	char* tmp;
	char* arrayElement = strtok(input, ",");
	for (unsigned char i = 0; i < arrayLength; ++i) {
		result->data[i] = atof(arrayElement);
		arrayElement = strtok(NULL, ",");
	}

	result->coveringRadius = 0;
	result->parentDistance = 0;
	result->arrayLength = arrayLength;

	SET_VARSIZE(result, size);

	PG_RETURN_POINTER(result);
}

Datum mtree_float_array_output(PG_FUNCTION_ARGS) {
	mtree_float_array* output = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);

	unsigned char arrayLength = output->arrayLength;

	StringInfoData stringInfo;
	initStringInfo(&stringInfo);

	char tmp[64];
	for (unsigned char i = 0; i < arrayLength; ++i) {
		sprintf(tmp, "%f", output->data[i]);
		appendStringInfoString(&stringInfo, tmp);
		if (i != arrayLength - 1) {
			appendStringInfoChar(&stringInfo, ',');
		}
	}

	PG_RETURN_CSTRING(stringInfo.data);
}

Datum mtree_float_array_consistent(PG_FUNCTION_ARGS) {
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float_array* query = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	StrategyNumber strategyNumber = (StrategyNumber)PG_GETARG_UINT16(2);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);
	mtree_float_array* key = DatumGetMtreeInt8Array(entry->key);
	float distance = mtree_float_array_distance_internal(key, query);

	bool returnValue;
	if (GIST_LEAF(entry)) {
		*recheck = false;
		switch (strategyNumber) {
		case GIST_SN_SAME:
			returnValue = mtree_float_array_equals(key, query);
			break;
		case GIST_SN_OVERLAPS:
			returnValue = mtree_float_array_overlap_distance(key, query, &distance);
			break;
		case GIST_SN_CONTAINS:
			returnValue = mtree_float_array_contains_distance(key, query, &distance);
			break;
		case GIST_SN_CONTAINED_BY:
			returnValue = mtree_float_array_contained_distance(key, query, &distance);
			break;
		default:
			ereport(ERROR,
				errcode(ERRCODE_SYNTAX_ERROR),
				errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
			break;
		}
	}
	else {
		switch (strategyNumber) {
		case GIST_SN_SAME:
			returnValue = mtree_float_array_contains_distance(key, query, &distance);
			*recheck = true;
			break;
		case GIST_SN_OVERLAPS:
			returnValue = mtree_float_array_overlap_distance(key, query, &distance);
			*recheck = !mtree_float_array_contained_distance(key, query, &distance);
			break;
		case GIST_SN_CONTAINS:
			returnValue = mtree_float_array_contains_distance(key, query, &distance);
			*recheck = true;
			break;
		case GIST_SN_CONTAINED_BY:
			returnValue = mtree_float_array_overlap_distance(key, query, &distance);
			*recheck = !mtree_float_array_contained_distance(key, query, &distance);
			break;
		default:
			ereport(ERROR,
				errcode(ERRCODE_SYNTAX_ERROR),
				errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
			break;
		}
	}

	PG_RETURN_BOOL(returnValue);
}

Datum mtree_float_array_union(PG_FUNCTION_ARGS) {
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GISTENTRY* entry = entryVector->vector;
	int ranges = entryVector->n;

	mtree_float_array* entries[ranges];
	for (int i = 0; i < ranges; ++i) {
		entries[i] = DatumGetMtreeInt8Array(entry[i].key);
	}

	int searchRange;

	switch (UNION_STRATEGY_FLOAT_ARRAY) {
	case First:
		searchRange = 1;
		break;
	case MinMaxDistance:
		searchRange = ranges;
		break;
	default:
		ereport(ERROR,
			errcode(ERRCODE_SYNTAX_ERROR),
			errmsg("Invalid StrategyNumber for union function: %u", UNION_STRATEGY_FLOAT_ARRAY));
		break;
	}

	int coveringRadii[searchRange];

	for (int i = 0; i < searchRange; ++i) {
		coveringRadii[i] = 0;

		for (int j = 0; j < ranges; ++j) {
			float distance = mtree_float_array_distance_internal(entries[i], entries[j]);
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

	mtree_float_array* out = mtree_float_array_deep_copy(entries[minimumIndex]);
	out->coveringRadius = coveringRadii[minimumIndex];

	PG_RETURN_MTREE_FLOAT_ARRAY_P(out);
}

Datum mtree_float_array_same(PG_FUNCTION_ARGS) {
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	PG_RETURN_BOOL(mtree_float_array_equals(first, second));
}

Datum mtree_float_array_penalty(PG_FUNCTION_ARGS) {
	GISTENTRY* originalEntry = (GISTENTRY*)PG_GETARG_POINTER(0);
	GISTENTRY* newEntry = (GISTENTRY*)PG_GETARG_POINTER(1);
	float* penalty = (float*)PG_GETARG_POINTER(2);
	mtree_float_array* original = DatumGetMtreeInt8Array(originalEntry->key);
	mtree_float_array* new = DatumGetMtreeInt8Array(newEntry->key);

	float distance = mtree_float_array_distance_internal(original, new);
	int newCoveringRadius = distance + new->coveringRadius;
	*penalty = (float)(newCoveringRadius < original->coveringRadius ? 0 : newCoveringRadius - original->coveringRadius);

	PG_RETURN_POINTER(penalty);
}

/* TODO: Lots of duplicate code. */
Datum mtree_float_array_picksplit(PG_FUNCTION_ARGS) {
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

	mtree_float_array* entries[maxOffset];
	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		entries[i - FirstOffsetNumber] = DatumGetMtreeInt8Array(entryVector->vector[i].key);
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

	switch (PICKSPLIT_STRATEGY_FLOAT_ARRAY) {
	case Random:
		leftIndex = ((int)random()) % (maxOffset - 1);
		rightIndex = (leftIndex + 1) + (((int)random()) % (maxOffset - leftIndex - 1));
		break;
	case FirstTwo:
		leftIndex = 0;
		rightIndex = 1;
		break;
	case MaxDistanceFromFirst:
		maxDistance = -1;
		for (int r = 0; r < maxOffset; ++r) {
			int distance = get_float_array_distance(maxOffset, entries, distances, 0, r);
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
				int distance = get_float_array_distance(maxOffset, entries, distances, l, r);
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
			rightCandidateIndex = (leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
				int leftDistance = get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
				int rightDistance = get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex = (leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; ++currentIndex) {
				int leftDistance = get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
				int rightDistance = get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

			if (minCoveringMax == -1 || MAX_2(leftRadius, rightRadius) < minCoveringMax) {
				minCoveringMax = MAX_2(leftRadius, rightRadius);
				leftIndex = leftCandidateIndex;
				rightIndex = rightCandidateIndex;
			}
		}
		break;
	case SamplingMinOverlapArea:
		for (int i = 0; i < trialCount; i++) {
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex = (leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int distance = get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
				int leftDistance = get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
				int rightDistance = get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
				minOverlapArea = (int)currentOverlapArea;
				leftIndex = leftCandidateIndex;
				rightIndex = rightCandidateIndex;
			}
		}
		break;
	case SamplingMinAreaSum:
		for (int i = 0; i < trialCount; i++) {
			leftCandidateIndex = ((int)random()) % (maxOffset - 1);
			rightCandidateIndex = (leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
			int leftRadius = 0, rightRadius = 0;

			for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
				int leftDistance = get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
				int rightDistance = get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
		ereport(ERROR,
			errcode(ERRCODE_SYNTAX_ERROR),
			errmsg("Invalid StrategyNumber for picksplit function: %u", PICKSPLIT_STRATEGY_FLOAT_ARRAY));
		break;
	}

	mtree_float_array* unionLeft = mtree_float_array_deep_copy(entries[leftIndex]);
	mtree_float_array* unionRight = mtree_float_array_deep_copy(entries[rightIndex]);
	mtree_float_array* current;

	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		int distanceLeft = get_float_array_distance(maxOffset, entries, distances, leftIndex, i - 1);
		int distanceRight = get_float_array_distance(maxOffset, entries, distances, rightIndex, i - 1);
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

Datum mtree_float_array_compress(PG_FUNCTION_ARGS) {
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_array_decompress(PG_FUNCTION_ARGS) {
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_array_distance(PG_FUNCTION_ARGS) {
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float_array* query = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	mtree_float_array* key = DatumGetMtreeInt8Array(entry->key);
	bool isLeaf = GistPageIsLeaf(entry->page);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);

	if (isLeaf) {
		*recheck = true;
	}

	PG_RETURN_FLOAT4((float4)mtree_float_array_distance_internal(query, key));
}

Datum mtree_float_array_distance_operator(PG_FUNCTION_ARGS) {
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);

	PG_RETURN_FLOAT4((float4)mtree_float_array_distance_internal(first, second));
}

Datum mtree_float_array_overlap_operator(PG_FUNCTION_ARGS) {
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);

	float res = mtree_float_array_distance_internal(first, second);
	bool result = mtree_float_array_overlap_distance(first, second, &res);

	PG_RETURN_BOOL(result);
}

Datum mtree_float_array_contains_operator(PG_FUNCTION_ARGS) {
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	float res = mtree_float_array_distance_internal(first, second);
	bool result = mtree_float_array_contains_distance(first, second, &res);

	PG_RETURN_BOOL(result);
}

Datum mtree_float_array_contained_operator(PG_FUNCTION_ARGS) {
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	float res = mtree_float_array_distance_internal(second, first);
	bool result = mtree_float_array_contains_distance(second, first, &res);

	PG_RETURN_BOOL(result);
}
