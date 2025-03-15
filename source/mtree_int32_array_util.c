/*
 * contrib/mtree_gist/mtree_int32_array_util.c
 */

#include "mtree_int32_array_util.h"

long long mtree_int32_array_outer_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	long long distance = int32_array_euclidean_distance(first, second);
	long long outer_distance = distance - first->coveringRadius - second->coveringRadius;

	if (outer_distance < 0) {
		outer_distance = 0;
	}

	return outer_distance;
}

long long mtree_int32_array_full_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	return int32_array_euclidean_distance(first, second);
}

bool mtree_int32_array_equals(mtree_int32_array* first, mtree_int32_array* second)
{
	if (first->arrayLength != second->arrayLength) {
		return false;
	}

	for (unsigned char i = 0; i < first->arrayLength; ++i) {
		if (first->data[i] != second->data[i]) {
			return false;
		}
	}

	return true;
}

bool mtree_int32_array_overlap_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	int32 full_distance = mtree_int32_array_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_int32_array_contains_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	int32 full_distance = mtree_int32_array_full_distance(first, second);
	return full_distance + second->coveringRadius < first->coveringRadius;
}

bool mtree_int32_array_contained_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	return mtree_int32_array_contains_distance(second, first);
}

mtree_int32_array* mtree_int32_array_deep_copy(mtree_int32_array* source)
{
	mtree_int32_array* destination = (mtree_int32_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

long long get_int32_array_distance(int size, mtree_int32_array* entries[size], long long distances[size][size], int i,
								   int j)
{
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_int32_array_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}

long long int32_simple_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	long long distance = 0;
	unsigned char minimumLength;  //, maximumLength;
	// mtree_int32_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
	} else {
		minimumLength = second->arrayLength;
	}

	for (unsigned char i = 0; i < minimumLength; ++i) {
		if (first->data[i] > second->data[i]) {
			--distance;
		} else {
			++distance;
		}
	}

	return distance;
}

long long int32_array_sum_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	long long distance = 0;
	unsigned char minimumLength, maximumLength;
	mtree_int32_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		maximumLength = second->arrayLength;
		longer = second;
	} else {
		minimumLength = second->arrayLength;
		maximumLength = first->arrayLength;
		longer = first;
	}

	for (unsigned char i = 0; i < minimumLength; ++i) {
		if (first->data[i] != 0 || second->data[i] != 0) {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])) /
						 (first->data[i] + second->data[i]));
		} else {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])));
		}
	}

	for (unsigned char i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i];
	}

	return distance;
}

long long int32_array_kullback_leibler_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	long long distance = 0;
	unsigned char minimumLength, maximumLength;
	mtree_int32_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		maximumLength = second->arrayLength;
		longer = second;
	} else {
		minimumLength = second->arrayLength;
		maximumLength = first->arrayLength;
		longer = first;
	}

	for (unsigned char i = 0; i < minimumLength; ++i) {
		if (second->data[i] != 0) {
			distance += ((first->data[i] - second->data[i]) * log(first->data[i] / second->data[i]));
		} else {
			distance += ((first->data[i] - second->data[i]) * log(first->data[i]));
		}
	}

	for (unsigned char i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i];
	}

	return distance;
}

long long int32_array_euclidean_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	float distance = 0.0;
	int minimumLength, maximumLength;
	mtree_int32_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		maximumLength = second->arrayLength;
		longer = second;
	} else {
		minimumLength = second->arrayLength;
		maximumLength = first->arrayLength;
		longer = first;
	}

	for (int i = 0; i < minimumLength; ++i) {
		distance += ((first->data[i] - second->data[i]) * (first->data[i] - second->data[i]));
	}

	for (int i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i] * longer->data[i];
	}

	elog(INFO, "Distance: %f", sqrt(distance));
	return sqrt(distance);
}
