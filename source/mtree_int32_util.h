/*
 * contrib/mtree_gist/mtree_int32_util.h
 */

#ifndef __MTREE_INT32_UTIL_H__
#define __MTREE_INT32_UTIL_H__

#include "mtree_int32.h"

long long mtree_int32_outer_distance(mtree_int32* first, mtree_int32* second);
long long mtree_int32_full_distance(mtree_int32* first, mtree_int32* second);
bool mtree_int32_equals(mtree_int32* first, mtree_int32* second);
bool mtree_int32_overlap_distance(mtree_int32* first, mtree_int32* second);
bool mtree_int32_contains_distance(mtree_int32* first, mtree_int32* second);
bool mtree_int32_contained_distance(mtree_int32* first, mtree_int32* second);
mtree_int32* mtree_int32_deep_copy(mtree_int32* source);
long long get_int32_distance(int size, mtree_int32* entries[size], long long distances[size][size], int i, int j);

#endif
