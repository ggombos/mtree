/*
 * contrib/mtree_gist/mtree_int8_array_util.c
 */

#include "mtree_int8_array_util.h"

long long mtree_int8_array_distance_internal(mtree_int8_array* first, mtree_int8_array* second) {
	long long distance = int8_array_taxicab_distance(first, second);
	long long retval = (distance - first->coveringRadius) - second->coveringRadius;
	
	if (retval < 0) {
		retval = 0;
	}

	return retval;
}

bool mtree_int8_array_equals(mtree_int8_array* first, mtree_int8_array* second) {
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

bool mtree_int8_array_overlap_distance(mtree_int8_array* first, mtree_int8_array* second, int distance) {
	return distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_int8_array_contains_distance(mtree_int8_array* first, mtree_int8_array* second, int distance) {
	return first->coveringRadius >= distance + second->coveringRadius;
}

bool mtree_int8_array_contained_distance(mtree_int8_array* first, mtree_int8_array* second, int distance) {
	return mtree_int8_array_contains_distance(second, first, distance);
}

mtree_int8_array* mtree_int8_array_deep_copy(mtree_int8_array* source) {
	mtree_int8_array* destination = (mtree_int8_array*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

long long get_int8_array_distance(int size, mtree_int8_array* entries[size], long long distances[size][size], int i, int j) {
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_int8_array_distance_internal(entries[i], entries[j]);
	}
	return distances[i][j];
}

int int8_simple_distance(mtree_int8_array* first, mtree_int8_array* second) {
	int distance = 0;
	unsigned char minimumLength;//, maximumLength;
	//mtree_int8_array* longer;

	if (first->arrayLength <= second->arrayLength) {
		minimumLength = first->arrayLength;
		//maximumLength = second->arrayLength;
		//longer = second;
	}
	else {
		minimumLength = second->arrayLength;
		//maximumLength = first->arrayLength;
		//longer = first;
	}

	for (unsigned char i = 0; i < minimumLength; ++i) {
		if (first->data[i] > second->data[i]) {
			--distance;
		}
		else {
			++distance;
		}
	}

	return distance;
}

int int8_array_sum_distance(mtree_int8_array* first, mtree_int8_array* second) {
	int distance = 0;
	unsigned char minimumLength, maximumLength;
	mtree_int8_array* longer;

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
		if (first->data[i] != 0 || second->data[i] != 0) {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])) / (first->data[i] + second->data[i]));
		}
		else {
			distance += (((first->data[i] - second->data[i]) * (first->data[i] - second->data[i])));
		}
	}

	for (unsigned char i = minimumLength; i < maximumLength; ++i) {
		distance += longer->data[i];
	}

	return distance;
}

int64 int8_array_kullback_leibler_distance(mtree_int8_array* first, mtree_int8_array* second) {
	int64 distance = 0;
	unsigned char minimumLength, maximumLength;
	mtree_int8_array* longer;

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

long long int8_array_taxicab_distance(mtree_int8_array* first, mtree_int8_array* second) {
	long long distance = 0;
	
	for (int i = 0; i < first->arrayLength; i++){
		distance += llabs(first->data[i] - second->data[i]);
	}

	return distance;
}
