/*
 * contrib/mtree_gist/mtree_text_array_util.h
 */

#ifndef __MTREE_TEXT_ARRAY_UTIL_H__
#define __MTREE_TEXT_ARRAY_UTIL_H__

#include "mtree_text_array.h"

float mtree_text_array_distance_internal(mtree_text_array* first, mtree_text_array* second, PG_FUNCTION_ARGS);
float weighted_text_array_distance(mtree_text_array* first, mtree_text_array* second);
int simple_text_array_distance(mtree_text_array* first, mtree_text_array* second);
bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second);
bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second, float* distance);
bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second, float* distance);
bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second, float* distance);
mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source);
float get_text_array_distance(int size, mtree_text_array* entries[size], float distances[size][size], int i, int j, PG_FUNCTION_ARGS);

#endif
