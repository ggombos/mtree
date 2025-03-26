/*
 * contrib/mtree_gist/mtree_float_util.h
 */

#ifndef __MTREE_FLOAT_UTIL_H__
#define __MTREE_FLOAT_UTIL_H__

#include "mtree_float.h"

double mtree_float_full_distance(mtree_float* first, mtree_float* second);
double mtree_float_outer_distance(mtree_float* first, mtree_float* second);
bool mtree_float_equals(mtree_float* first, mtree_float* second);
bool mtree_float_overlap_distance(mtree_float* first, mtree_float* second);
bool mtree_float_contains_distance(mtree_float* first, mtree_float* second);
bool mtree_float_contained_distance(mtree_float* first, mtree_float* second);
mtree_float* mtree_float_deep_copy(mtree_float* source);
double get_float_distance(int size, mtree_float* entries[size], double distances[size][size], int i, int j);

#endif
