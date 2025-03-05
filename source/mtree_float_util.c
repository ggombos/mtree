/*
 * contrib/mtree_gist/mtree_float_util.c
 */

#include "mtree_float_util.h"


float mtree_float_outer_distance(mtree_float* first, mtree_float* second) {
	float distance = fabs(first->data - second->data);
	float outer_distance = distance - first->coveringRadius - second->coveringRadius;
	
	if (outer_distance < 0.0){
		outer_distance = 0.0;
	}
	
	return outer_distance;
}

float mtree_float_full_distance(mtree_float* first, mtree_float* second) {
	return fabs(first->data - second->data);
}

bool mtree_float_equals(mtree_float* first, mtree_float* second) {
	return first->data == second->data;
}

bool mtree_float_overlap_distance(mtree_float* first, mtree_float* second) {
	float full_distance = mtree_float_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_float_contains_distance(mtree_float* first, mtree_float* second) {
	float full_distance = mtree_float_full_distance(first, second);
	return full_distance + second->coveringRadius < first->coveringRadius;
}

bool mtree_float_contained_distance(mtree_float* first, mtree_float* second) {
	return mtree_float_contains_distance(second, first);
}

mtree_float* mtree_float_deep_copy(mtree_float* source) {
	mtree_float* destination = (mtree_float*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

float get_float_distance(int size, mtree_float* entries[size], float distances[size][size], int i, int j) {
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_float_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}
