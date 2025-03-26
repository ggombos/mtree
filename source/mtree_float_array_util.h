/*
 * contrib/mtree_gist/mtree_float_array_util.h
 */

#ifndef __MTREE_FLOAT_ARRAY_UTIL_H__
#define __MTREE_FLOAT_ARRAY_UTIL_H__

#include "mtree_float_array.h"

double mtree_float_array_outer_distance(mtree_float_array* first, mtree_float_array* second);
double mtree_float_array_full_distance(mtree_float_array* first, mtree_float_array* second);
bool mtree_float_array_equals(mtree_float_array* first, mtree_float_array* second);
bool mtree_float_array_overlap_distance(mtree_float_array* first, mtree_float_array* second);
bool mtree_float_array_contains_distance(mtree_float_array* first, mtree_float_array* second);
bool mtree_float_array_contained_distance(mtree_float_array* first, mtree_float_array* second);
mtree_float_array* mtree_float_array_deep_copy(mtree_float_array* source);
double get_float_array_distance(int size, mtree_float_array* entries[size], double distances[size][size], int i, int j);

double float_array_sum_distance(mtree_float_array* first, mtree_float_array* second);
double float_array_kullback_leibler_distance(mtree_float_array* first, mtree_float_array* second);
double float_array_euclidean_distance(mtree_float_array* first, mtree_float_array* second);
double float_array_taxicab_distance(mtree_float_array* first, mtree_float_array* second);

#endif
