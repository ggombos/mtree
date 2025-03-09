/*
 * contrib/mtree_gist/mtree_float_array.c
 */

#include "mtree_float_array.h"

#include "mtree_float_array_util.h"
#include "mtree_util.h"

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
PG_FUNCTION_INFO_V1(mtree_float_array_radius);
PG_FUNCTION_INFO_V1(mtree_float_array_overlap_operator);

Datum mtree_float_array_input(PG_FUNCTION_ARGS)
{
	char* input = PG_GETARG_CSTRING(0);
	unsigned char inputLength = strlen(input);

	if (inputLength == 0) {
		ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR), errmsg("The input is an empty string."));
	}

	char previousInteger = '\0';
	unsigned char arrayLength = 1;
	for (unsigned char i = 0; i < inputLength; ++i) {
		if (isblank(input[i])) {
			ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR), errmsg("The array can not contain space or tab characters."));
		} else if (input[i] == ',' && previousInteger != '\0') {
			++arrayLength;
			previousInteger = '\0';
		} else if (!isdigit(input[i]) && input[i] != '-' && input[i] != '.' && input[i] != 'l') {
			ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
					errmsg("The array can only contain integers [0-9] and commas [,]."));
		}
		previousInteger = input[i];
	}

	size_t size = MTREE_FLOAT_ARRAY_SIZE + arrayLength * sizeof(int64) + 1;
	mtree_float_array* result = (mtree_float_array*)palloc(size);

	char* arrayElement = strtok(input, ",");
	if (*arrayElement == 'l') {
		arrayElement = strtok(NULL, ",");
		result->level = atoi(arrayElement);
		arrayElement = strtok(NULL, ",");
		arrayLength -= 2;
	} else {
		result->level = 0;
	}
	if (arrayElement == NULL) {
		ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR), errmsg("The input is an empty string."));
	}
	for (int i = 0; i < arrayLength; ++i) {
		result->data[i] = atof(arrayElement);
		arrayElement = strtok(NULL, ",");
	}

	result->coveringRadius = 0.0;
	result->parentDistance = 0.0;
	result->arrayLength = arrayLength;

	SET_VARSIZE(result, size);

	PG_RETURN_POINTER(result);
}

Datum mtree_float_array_output(PG_FUNCTION_ARGS)
{
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

Datum mtree_float_array_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float_array* query = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	StrategyNumber strategyNumber = (StrategyNumber)PG_GETARG_UINT16(2);
	bool* recheck = (bool*)PG_GETARG_POINTER(4);
	mtree_float_array* key = DatumGetMtreeFloatArray(entry->key);
	// float distance = mtree_float_array_outer_distance(key, query);

	*recheck = false;

	bool returnValue;
	if (GIST_LEAF(entry)) {
		switch (strategyNumber) {
			case GIST_SN_SAME:
				returnValue = mtree_float_array_equals(key, query);
				break;
			case GIST_SN_OVERLAPS:
				returnValue = mtree_float_array_overlap_distance(key, query);
				break;
			case GIST_SN_CONTAINS:
				returnValue = mtree_float_array_contains_distance(key, query);
				break;
			case GIST_SN_CONTAINED_BY:
				returnValue = mtree_float_array_contained_distance(key, query);
				break;
			default:
				ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
				break;
		}
	} else {
		switch (strategyNumber) {
			case GIST_SN_SAME:
				returnValue = mtree_float_array_contains_distance(key, query);
				break;
			case GIST_SN_OVERLAPS:
				returnValue = mtree_float_array_overlap_distance(key, query);
				break;
			case GIST_SN_CONTAINS:
				returnValue = mtree_float_array_contains_distance(key, query);
				break;
			case GIST_SN_CONTAINED_BY:
				returnValue = mtree_float_array_overlap_distance(key, query);
				break;
			default:
				ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
						errmsg("Invalid StrategyNumber for consistent function: %u", strategyNumber));
				break;
		}
	}

	PG_RETURN_BOOL(returnValue);
}

Datum mtree_float_array_union(PG_FUNCTION_ARGS)
{
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GISTENTRY* entry = entryVector->vector;
	int ranges = entryVector->n;
	// elog(INFO, "RANGES: %i", ranges);

	mtree_float_array* entries[ranges];
	for (int i = 0; i < ranges; ++i) {
		entries[i] = DatumGetMtreeFloatArray(entry[i].key);
		// elog(INFO, "%f, %f, %f", entries[i]->data[0], entries[i]->data[1], entries[i]->data[2]);
	}

	// int searchRange;

	// MtreeUnionStrategy UNION_STRATEGY_FLOAT_ARRAY = MinMaxDistance;
	// if (PG_HAS_OPCLASS_OPTIONS())
	// {
	// 	MtreeOptions* options = (MtreeOptions *) PG_GET_OPCLASS_OPTIONS();
	// 	UNION_STRATEGY_FLOAT_ARRAY = options->union_strategy;
	// }

	// switch (UNION_STRATEGY_FLOAT_ARRAY) {
	// case First:
	// 	searchRange = 1;
	// 	break;
	// case MinMaxDistance:
	// 	searchRange = ranges;
	// 	break;
	// default:
	// 	ereport(ERROR,
	// 		errcode(ERRCODE_SYNTAX_ERROR),
	// 		errmsg("Invalid StrategyNumber for union function: %u", UNION_STRATEGY_FLOAT_ARRAY));
	// 	break;
	// }

	// float coveringRadii[searchRange];

	// for (int i = 0; i < searchRange; ++i) {
	// 	coveringRadii[i] = 0.0;

	// 	for (int j = 0; j < ranges; ++j) {
	// 		float distance = mtree_float_array_outer_distance(entries[i], entries[j]);
	// 		float newCoveringRadius;

	// 		if (distance > 0){
	// 			newCoveringRadius = distance + entries[i]->coveringRadius + (2 * entries[j]->coveringRadius);
	// 		} else {
	// 			float exact_distance = mtree_float_array_full_distance(entries[i], entries[j]);
	// 			float intersect = exact_distance - (entries[i]->coveringRadius + entries[j]->coveringRadius);
	// 			newCoveringRadius = entries[i]->coveringRadius + (2 * entries[j]->coveringRadius) + intersect;
	// 		}

	// 		if (coveringRadii[i] < newCoveringRadius) {
	// 			coveringRadii[i] = newCoveringRadius;
	// 		}
	// 	}
	// }

	// int minimumIndex = 0;

	// for (int i = 1; i < searchRange; ++i) {
	// 	if (coveringRadii[i] < coveringRadii[minimumIndex]) {
	// 		minimumIndex = i;
	// 	}
	// }

	// mtree_float_array* out = mtree_float_array_deep_copy(entries[minimumIndex]);
	// out->coveringRadius = coveringRadii[minimumIndex];

	mtree_float_array* out = mtree_float_array_deep_copy(entries[0]);
	out->coveringRadius += mtree_float_array_outer_distance(entries[0], entries[1]);

	// elog(INFO, "SELECTED: %f, %f, %f", out->data[0], out->data[1], out->data[2]);

	PG_RETURN_MTREE_FLOAT_ARRAY_P(out);
}

Datum mtree_float_array_same(PG_FUNCTION_ARGS)
{
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	PG_RETURN_BOOL(mtree_float_array_equals(first, second));
}

Datum mtree_float_array_penalty(PG_FUNCTION_ARGS)
{
	GISTENTRY* originalEntry = (GISTENTRY*)PG_GETARG_POINTER(0);
	GISTENTRY* newEntry = (GISTENTRY*)PG_GETARG_POINTER(1);
	float* penalty = (float*)PG_GETARG_POINTER(2);
	mtree_float_array* original = DatumGetMtreeFloatArray(originalEntry->key);
	mtree_float_array* new = DatumGetMtreeFloatArray(newEntry->key);

	float distance = mtree_float_array_outer_distance(original, new);
	*penalty = distance;

	PG_RETURN_POINTER(penalty);
}

/* TODO: Lots of duplicate code. */
Datum mtree_float_array_picksplit(PG_FUNCTION_ARGS)
{
	GistEntryVector* entryVector = (GistEntryVector*)PG_GETARG_POINTER(0);
	GIST_SPLITVEC* vector = (GIST_SPLITVEC*)PG_GETARG_POINTER(1);
	OffsetNumber maxOffset = (OffsetNumber)entryVector->n - 1;
	OffsetNumber numberBytes = (OffsetNumber)(maxOffset + 1) * sizeof(OffsetNumber);
	OffsetNumber* left;
	OffsetNumber* right;

	// elog(INFO, "OFFSET: %i", maxOffset);

	vector->spl_left = (OffsetNumber*)palloc(numberBytes);
	left = vector->spl_left;
	vector->spl_nleft = 0;

	vector->spl_right = (OffsetNumber*)palloc(numberBytes);
	right = vector->spl_right;
	vector->spl_nright = 0;

	mtree_float_array* entries[maxOffset];

	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		entries[i - FirstOffsetNumber] = DatumGetMtreeFloatArray(entryVector->vector[i].key);
	}

	float distances[maxOffset][maxOffset];
	init_distances_float(maxOffset, *distances);

	int leftIndex, rightIndex, leftCandidateIndex, rightCandidateIndex;
	int trialCount = 100;
	float maxDistance = -1;
	float minCoveringSum = -1.0;
	float minCoveringMax = -1.0;
	float minOverlapArea = -1.0;
	float minSumArea = -1.0;

	MtreePickSplitStrategy PICKSPLIT_STRATEGY_FLOAT_ARRAY = SamplingMinOverlapArea;
	if (PG_HAS_OPCLASS_OPTIONS()) {
		MtreeOptions* options = (MtreeOptions*)PG_GET_OPCLASS_OPTIONS();
		PICKSPLIT_STRATEGY_FLOAT_ARRAY = options->picksplit_strategy;
	}

	switch (PICKSPLIT_STRATEGY_FLOAT_ARRAY) {
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
				float distance = get_float_array_distance(maxOffset, entries, distances, 0, r);
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
					float distance = get_float_array_distance(maxOffset, entries, distances, l, r);
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
				float leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					float distanceLeft =
						get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					float distanceRight =
						get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
				leftCandidateIndex = (random()) % (maxOffset - 1);
				rightCandidateIndex =
					(leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
				float leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; ++currentIndex) {
					float distanceLeft =
						get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					float distanceRight =
						get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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
				float distance =
					get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, rightCandidateIndex);
				float leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					float distanceLeft =
						get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					float distanceRight =
						get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

				double currentOverlapArea = overlap_area_float(leftRadius, rightRadius, distance);
				if (minOverlapArea == -1.0 || currentOverlapArea < minOverlapArea) {
					minOverlapArea = (float)currentOverlapArea;
					leftIndex = leftCandidateIndex;
					rightIndex = rightCandidateIndex;
				}
			}
			break;
		case GuttmanPolyTime:
			for (int i = 0; i < maxOffset; i++) {
				for (int j = i + 1; j < maxOffset; j++) {
					leftCandidateIndex = i;
					rightCandidateIndex = j;
					float distance = get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex,
															  rightCandidateIndex);
					float leftRadius = 0.0, rightRadius = 0.0;

					for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
						float distanceLeft =
							get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
						float distanceRight =
							get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

					double currentOverlapArea = overlap_area_float(leftRadius, rightRadius, distance);
					if ((minOverlapArea == -1.0 || currentOverlapArea < minOverlapArea) &&
						(currentOverlapArea != 0.0)) {
						minOverlapArea = (float)currentOverlapArea;
						elog(INFO, "I: %d, J: %d, LEFTRADIUS: %f, RIGHTRADIUS: %f, DISTANCE: %f", i, j, leftRadius,
							 rightRadius, distance);
						elog(INFO, "OVERLAPAREA: %f, %f", currentOverlapArea, minOverlapArea);
						leftIndex = leftCandidateIndex;
						rightIndex = rightCandidateIndex;
					}
				}
			}
			break;
		case SamplingMinAreaSum:
			for (int i = 0; i < trialCount; i++) {
				leftCandidateIndex = ((int)random()) % (maxOffset - 1);
				rightCandidateIndex =
					(leftCandidateIndex + 1) + (((int)random()) % (maxOffset - leftCandidateIndex - 1));
				float leftRadius = 0.0, rightRadius = 0.0;

				for (int currentIndex = 0; currentIndex < maxOffset; currentIndex++) {
					float distanceLeft =
						get_float_array_distance(maxOffset, entries, distances, leftCandidateIndex, currentIndex);
					float distanceRight =
						get_float_array_distance(maxOffset, entries, distances, rightCandidateIndex, currentIndex);

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

				float currentSumArea = leftRadius * leftRadius + rightRadius * rightRadius;
				if (minSumArea == -1.0 || currentSumArea < minSumArea) {
					minSumArea = currentSumArea;
					leftIndex = leftCandidateIndex;
					rightIndex = rightCandidateIndex;
				}
			}
			break;
		default:
			ereport(ERROR, errcode(ERRCODE_SYNTAX_ERROR),
					errmsg("Invalid StrategyNumber for picksplit function: %u", PICKSPLIT_STRATEGY_FLOAT_ARRAY));
			break;
	}

	mtree_float_array* unionLeft = mtree_float_array_deep_copy(entries[leftIndex]);
	mtree_float_array* unionRight = mtree_float_array_deep_copy(entries[rightIndex]);
	mtree_float_array* current;

	// int left_size = 0;
	// int right_rize = 0;

	for (OffsetNumber i = FirstOffsetNumber; i <= maxOffset; i = OffsetNumberNext(i)) {
		float distanceLeft = get_float_array_distance(maxOffset, entries, distances, leftIndex, i - 1);
		float distanceRight = get_float_array_distance(maxOffset, entries, distances, rightIndex, i - 1);
		current = entries[i - 1];

		if (distanceLeft < distanceRight) {
			if (distanceLeft + current->coveringRadius > unionLeft->coveringRadius) {
				unionLeft->coveringRadius = distanceLeft + current->coveringRadius;
			}
			// elog(INFO, "B: %f, %f, %f", current->data[0], current->data[1], current->data[2]);
			*left = i;
			++left;
			++(vector->spl_nleft);

			// left_size++;
		} else {
			if (distanceRight + current->coveringRadius > unionRight->coveringRadius) {
				unionRight->coveringRadius = distanceRight + current->coveringRadius;
			}
			// elog(INFO, "J: %f, %f, %f", current->data[0], current->data[1], current->data[2]);
			*right = i;
			++right;
			++(vector->spl_nright);

			// right_rize++;
		}
	}

	vector->spl_ldatum = PointerGetDatum(unionLeft);
	vector->spl_rdatum = PointerGetDatum(unionRight);

	// elog(INFO, "BELSO CSUCS B: %f, %f, %f, %f", unionLeft->data[0], unionLeft->data[1], unionLeft->data[2],
	// unionLeft->coveringRadius); elog(INFO, "BELSO CSUCS J: %f, %f, %f, %f", unionRight->data[0], unionRight->data[1],
	// unionRight->data[2], unionRight->coveringRadius); elog(INFO, "MAX OFFSET: %d, LEFT SIZE: %d, RIGHT SIZE: %d",
	// maxOffset, left_size, right_rize);

	PG_RETURN_POINTER(vector);
}

Datum mtree_float_array_compress(PG_FUNCTION_ARGS)
{
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_array_decompress(PG_FUNCTION_ARGS)
{
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

Datum mtree_float_array_distance(PG_FUNCTION_ARGS)
{
	GISTENTRY* entry = (GISTENTRY*)PG_GETARG_POINTER(0);
	mtree_float_array* query = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	mtree_float_array* key = DatumGetMtreeFloatArray(entry->key);

	PG_RETURN_FLOAT4((float4)mtree_float_array_outer_distance(query, key));
}

Datum mtree_float_array_distance_operator(PG_FUNCTION_ARGS)
{
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);

	PG_RETURN_FLOAT4((float4)mtree_float_array_outer_distance(first, second));
}

Datum mtree_float_array_radius(PG_FUNCTION_ARGS)
{
	mtree_float_array* node = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);

	PG_RETURN_FLOAT4((float4)node->coveringRadius);
}

Datum mtree_float_array_overlap_operator(PG_FUNCTION_ARGS)
{
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);

	bool result = mtree_float_array_overlap_distance(first, second);

	PG_RETURN_BOOL(result);
}

Datum mtree_float_array_contains_operator(PG_FUNCTION_ARGS)
{
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	bool result = mtree_float_array_contains_distance(first, second);

	PG_RETURN_BOOL(result);
}

Datum mtree_float_array_contained_operator(PG_FUNCTION_ARGS)
{
	mtree_float_array* first = PG_GETARG_MTREE_FLOAT_ARRAY_P(0);
	mtree_float_array* second = PG_GETARG_MTREE_FLOAT_ARRAY_P(1);
	bool result = mtree_float_array_contains_distance(second, first);

	PG_RETURN_BOOL(result);
}
