/*
 * contrib/mtree_gist/mtree_int32_array_util.c
 */

#include "mtree_int32_array_util.h"

double mtree_int32_array_outer_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	double distance = int32_array_euclidean_distance(first, second);
	double outer_distance = distance - first->coveringRadius - second->coveringRadius;

	if (outer_distance < 0.0) {
		outer_distance = 0.0;
	}

	return outer_distance;
}

double mtree_int32_array_full_distance(mtree_int32_array* first, mtree_int32_array* second)
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
	double full_distance = mtree_int32_array_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_int32_array_contains_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	double full_distance = mtree_int32_array_full_distance(first, second);
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

double get_int32_array_distance(int size, mtree_int32_array* entries[size], double distances[size][size], int i, int j)
{
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_int32_array_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}

double int32_simple_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	double distance = 0.0;
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

double int32_array_sum_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	double distance = 0.0;
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

	return fabs(distance);
}

double int32_array_kullback_leibler_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	double distance = 0.0;
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

double int32_array_euclidean_distance(mtree_int32_array* first, mtree_int32_array* second)
{
	double distance = 0.0;
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
		double diff = (double)first->data[i] - (double)second->data[i];
		distance += diff * diff;
	}

	for (int i = minimumLength; i < maximumLength; ++i) {
		double value = (double)(longer->data[i]);
		distance += value * value;
	}

	return sqrt(distance);
}
