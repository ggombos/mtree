/*
 * contrib/mtree_gist/mtree_float_array_util.c
 */

#include "mtree_float_array_util.h"

float mtree_float_array_distance_internal(mtree_float_array* first, mtree_float_array* second) {
	// elog(INFO, "dist: %f %f %f %f", first->data, second->data, float_array_sum_distance(first, second), float_array_kullback_leibler_distance(first, second));
	// return float_array_kullback_leibler_distance(first, second);
	return float_array_sum_distance(first, second);
}

bool mtree_float_array_equals(mtree_float_array* first, mtree_float_array* second) {
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

bool mtree_float_array_overlap_distance(mtree_float_array* first, mtree_float_array* second, float* distance) {
	// TODO: Implement overlap distance
	return false;
}

bool mtree_float_array_contains_distance(mtree_float_array* first, mtree_float_array* second, float* distance) {
	// TODO: Implement contains distance
	return false;
}

bool mtree_float_array_contained_distance(mtree_float_array* first, mtree_float_array* second, float* distance) {
	return mtree_float_array_contains_distance(second, first, distance);
}

mtree_float_array* mtree_float_array_deep_copy(mtree_float_array* source) {
	mtree_float_array* destination = (mtree_float_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

int get_float_array_distance(int size, mtree_float_array* entries[size], int distances[size][size], int i, int j) {
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_float_array_distance_internal(entries[i], entries[j]);
	}
	return distances[i][j];
}

float float_array_sum_distance(mtree_float_array* first, mtree_float_array* second) {
	float distance = 0.0;
	unsigned char minimumLength, maximumLength;
	mtree_float_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		maximumLength = second->arrayLength;
		longer = second;
	}
	else {
		minimumLength = second->arrayLength;
		maximumLength = first->arrayLength;
		longer = first;
	}

	for (unsigned char i = 0; i < minimumLength; ++i) {
		if (first->data[i] + second->data[i] != 0.0) {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])) / (first->data[i] + second->data[i]));
		}
		else {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])));
		}
	}

	for (unsigned char i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i];
	}
	// TODO: EZ ITT KERDES: a tavolsag nem lehet negativ, jo ez igy?
	return fabs(distance);
}

float float_array_kullback_leibler_distance(mtree_float_array* first, mtree_float_array* second) {
	float distance = 0;
	unsigned char minimumLength, maximumLength;
	mtree_float_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		maximumLength = second->arrayLength;
		longer = second;
	}
	else {
		minimumLength = second->arrayLength;
		maximumLength = first->arrayLength;
		longer = first;
	}

	for (unsigned char i = 0; i < minimumLength; ++i) {
		if (second->data[i] != 0) {
			distance += ((first->data[i] - second->data[i]) * log(first->data[i] / second->data[i]));
		}
		else {
			distance += ((first->data[i] - second->data[i]) * log(first->data[i]));
		}
	}

	for (unsigned char i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i];
	}

	return distance;
}
