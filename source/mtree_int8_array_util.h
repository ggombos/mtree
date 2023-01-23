/*
 * contrib/mtree_gist/mtree_int8_array_util.h
 */

#ifndef __MTREE_INT8_ARRAY_UTIL_H__
#define __MTREE_INT8_ARRAY_UTIL_H__

#include "mtree_int8_array.h"

int64 mtree_int8_array_distance_internal(mtree_int8_array* first, mtree_int8_array* second);
bool mtree_int8_array_equals(mtree_int8_array* first, mtree_int8_array* second);
bool mtree_int8_array_overlap_distance(mtree_int8_array* first, mtree_int8_array* second, int distance);
bool mtree_int8_array_contains_distance(mtree_int8_array* first, mtree_int8_array* second, int distance);
bool mtree_int8_array_contained_distance(mtree_int8_array* first, mtree_int8_array* second, int distance);
mtree_int8_array* mtree_int8_array_deep_copy(mtree_int8_array* source);
int get_int8_array_distance(int size, mtree_int8_array* entries[size], int distances[size][size], int i, int j);

int int8_simple_distance(mtree_int8_array* first, mtree_int8_array* second);
int int8_array_sum_distance(mtree_int8_array* first, mtree_int8_array* second);
int64 int8_array_kullback_leibler_distance(mtree_int8_array* first, mtree_int8_array* second);

#endif
