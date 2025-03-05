/*
 * contrib/mtree_gist/mtree_int64_util.h
 */

#ifndef __MTREE_INT64_UTIL_H__
#define __MTREE_INT64_UTIL_H__

#include "mtree_int64.h"

long long mtree_int64_outer_distance(mtree_int64* first, mtree_int64* second);
long long mtree_int64_full_distance(mtree_int64* first, mtree_int64* second);
bool mtree_int64_equals(mtree_int64* first, mtree_int64* second);
bool mtree_int64_overlap_distance(mtree_int64* first, mtree_int64* second);
bool mtree_int64_contains_distance(mtree_int64* first, mtree_int64* second);
bool mtree_int64_contained_distance(mtree_int64* first, mtree_int64* second);
mtree_int64* mtree_int64_deep_copy(mtree_int64* source);
long long get_int64_distance(int size, mtree_int64* entries[size], long long distances[size][size], int i, int j);

#endif
