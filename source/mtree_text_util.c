/*
 * contrib/mtree_gist/mtree_text_util.c
 */

#include "mtree_text_util.h"
#include "mtree_util.h"

int mtree_text_string_distance(mtree_text* first, mtree_text* second) {
	return string_distance(first->vl_data, second->vl_data);
}

bool mtree_text_equals(mtree_text* first, mtree_text* second) {
	return strcmp(first->vl_data, second->vl_data) == 0;
}

bool mtree_text_overlap_distance(mtree_text* first, mtree_text* second, int distance) {
	return distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_text_contains_distance(mtree_text* first, mtree_text* second, int distance) {
	return first->coveringRadius >= distance + second->coveringRadius;
}

bool mtree_text_contained_distance(mtree_text* first, mtree_text* second, int distance) {
	return mtree_text_contains_distance(second, first, distance);
}

mtree_text* mtree_text_deep_copy(mtree_text* source) {
	mtree_text* destination = (mtree_text*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

int get_distance(int size, mtree_text* entries[size], int distances[size][size], int i, int j) {
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_text_string_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}

bool mtree_text_overlap_wrapper(mtree_text* first, mtree_text* second) {
	return mtree_text_overlap_distance(first, second, mtree_text_string_distance(first, second));
}

bool mtree_text_contains_wrapper(mtree_text* first, mtree_text* second) {
	return mtree_text_contains_distance(first, second, mtree_text_string_distance(first, second));
}

bool mtree_text_contained_wrapper(mtree_text* first, mtree_text* second) {
	return mtree_text_contains_wrapper(second, first);
}
