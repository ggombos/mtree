/*
 * contrib/mtree_gist/mtree_float_array_util.c
 */

#include "mtree_float_array_util.h"

#include <math.h>

float mtree_float_array_outer_distance(mtree_float_array* first, mtree_float_array* second)
{
	float distance = float_array_euclidean_distance(first, second);
	float outer_distance = distance - first->coveringRadius - second->coveringRadius;

	if (outer_distance < 0.0) {
		outer_distance = 0.0;
	}

	return outer_distance;
}

float mtree_float_array_full_distance(mtree_float_array* first, mtree_float_array* second)
{
	return float_array_euclidean_distance(first, second);
}

bool mtree_float_array_equals(mtree_float_array* first, mtree_float_array* second)
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

bool mtree_float_array_overlap_distance(mtree_float_array* first, mtree_float_array* second)
{
	float full_distance = mtree_float_array_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_float_array_contains_distance(mtree_float_array* first, mtree_float_array* second)
{
	float full_distance = mtree_float_array_full_distance(first, second);
	return full_distance + second->coveringRadius < first->coveringRadius;
}

bool mtree_float_array_contained_distance(mtree_float_array* first, mtree_float_array* second)
{
	return mtree_float_array_contains_distance(second, first);
}

mtree_float_array* mtree_float_array_deep_copy(mtree_float_array* source)
{
	mtree_float_array* destination = (mtree_float_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

float get_float_array_distance(int size, mtree_float_array* entries[size], float distances[size][size], int i, int j)
{
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_float_array_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}

float float_array_sum_distance(mtree_float_array* first, mtree_float_array* second)
{
	float distance = 0.0;
	unsigned char minimumLength, maximumLength;
	mtree_float_array* longer;

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
		if (first->data[i] + second->data[i] != 0.0) {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])) /
						 (first->data[i] + second->data[i]));
		} else {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])));
		}
	}

	for (unsigned char i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i];
	}

	return fabs(distance);
}

float float_array_kullback_leibler_distance(mtree_float_array* first, mtree_float_array* second)
{
	float distance = 0;
	unsigned char minimumLength, maximumLength;
	mtree_float_array* longer;

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

float float_array_euclidean_distance(mtree_float_array* first, mtree_float_array* second)
{
	float distance = 0.0;
	int minimumLength, maximumLength;
	mtree_float_array* longer;

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

	return sqrt(distance);
}

float float_array_taxicab_distance(mtree_float_array* first, mtree_float_array* second)
{
	float distance = 0.0;
	int minimumLength;	//, maximumLength;
	// mtree_float_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		// maximumLength = second->arrayLength;
		// longer = second;
	} else {
		minimumLength = second->arrayLength;
		// maximumLength = first->arrayLength;
		// longer = first;
	}

	for (int i = 0; i < minimumLength; ++i) {
		distance += fabs(first->data[i] - second->data[i]);
	}

	return distance;
}
