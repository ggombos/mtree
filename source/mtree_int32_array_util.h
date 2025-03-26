/*
 * contrib/mtree_gist/mtree_int32_array_util.h
 */

#ifndef __MTREE_INT32_ARRAY_UTIL_H__
#define __MTREE_INT32_ARRAY_UTIL_H__

#include "mtree_int32_array.h"

double mtree_int32_array_outer_distance(mtree_int32_array* first, mtree_int32_array* second);
double mtree_int32_array_full_distance(mtree_int32_array* first, mtree_int32_array* second);
bool mtree_int32_array_equals(mtree_int32_array* first, mtree_int32_array* second);
bool mtree_int32_array_overlap_distance(mtree_int32_array* first, mtree_int32_array* second);
bool mtree_int32_array_contains_distance(mtree_int32_array* first, mtree_int32_array* second);
bool mtree_int32_array_contained_distance(mtree_int32_array* first, mtree_int32_array* second);
mtree_int32_array* mtree_int32_array_deep_copy(mtree_int32_array* source);
double get_int32_array_distance(int size, mtree_int32_array* entries[size], double distances[size][size], int i, int j);

double int32_simple_distance(mtree_int32_array* first, mtree_int32_array* second);
double int32_array_sum_distance(mtree_int32_array* first, mtree_int32_array* second);
double int32_array_kullback_leibler_distance(mtree_int32_array* first, mtree_int32_array* second);
double int32_array_euclidean_distance(mtree_int32_array* first, mtree_int32_array* second);

#endif
