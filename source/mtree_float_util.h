/*
 * contrib/mtree_gist/mtree_float_util.h
 */

#ifndef __MTREE_FLOAT_UTIL_H__
#define __MTREE_FLOAT_UTIL_H__

#include "mtree_float.h"

float mtree_float_distance_internal(mtree_float* first, mtree_float* second);
bool mtree_float_equals(mtree_float* first, mtree_float* second);
bool mtree_float_overlap_distance(mtree_float* first, mtree_float* second, float* distance);
bool mtree_float_contains_distance(mtree_float* first, mtree_float* second, float* distance);
bool mtree_float_contained_distance(mtree_float* first, mtree_float* second, float* distance);
mtree_float* mtree_float_deep_copy(mtree_float* source);
int get_float_distance(int size, mtree_float* entries[size], int distances[size][size], int i, int j);

#endif
