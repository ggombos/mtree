/*
 * contrib/mtree_gist/mtree_text_util.h
 */

#ifndef __MTREE_TEXT_UTIL_H__
#define __MTREE_TEXT_UTIL_H__

#include "mtree_text.h"

double mtree_text_outer_distance(mtree_text* first, mtree_text* second);
double mtree_text_full_distance(mtree_text* first, mtree_text* second);
bool mtree_text_equals(mtree_text* first, mtree_text* second);
bool mtree_text_overlap_distance(mtree_text* first, mtree_text* second);
bool mtree_text_contains_distance(mtree_text* first, mtree_text* second);
bool mtree_text_contained_distance(mtree_text* first, mtree_text* second);
mtree_text* mtree_text_deep_copy(mtree_text* source);
double get_distance(int size, mtree_text* entries[size], double distances[size][size], int i, int j);
bool mtree_text_overlap_wrapper(mtree_text* first, mtree_text* second);
bool mtree_text_contains_wrapper(mtree_text* first, mtree_text* second);
bool mtree_text_contained_wrapper(mtree_text* first, mtree_text* second);

#endif
