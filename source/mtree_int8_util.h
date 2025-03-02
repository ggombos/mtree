/*
 * contrib/mtree_gist/mtree_int8_util.h
 */

#ifndef __MTREE_INT8_UTIL_H__
#define __MTREE_INT8_UTIL_H__

#include "mtree_int8.h"

long long mtree_int8_distance_internal(mtree_int8* first, mtree_int8* second);
bool mtree_int8_equals(mtree_int8* first, mtree_int8* second);
bool mtree_int8_overlap_distance(mtree_int8* first, mtree_int8* second, int distance);
bool mtree_int8_contains_distance(mtree_int8* first, mtree_int8* second, int distance);
bool mtree_int8_contained_distance(mtree_int8* first, mtree_int8* second, int distance);
mtree_int8* mtree_int8_deep_copy(mtree_int8* source);
long long get_int8_distance(int size, mtree_int8* entries[size], long long distances[size][size], int i, int j);

#endif
