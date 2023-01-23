/*
 * contrib/mtree_gist/mtree_text_array_util.h
 */

#ifndef __MTREE_TEXT_ARRAY_UTIL_H__
#define __MTREE_TEXT_ARRAY_UTIL_H__

#include "mtree_text_array.h"

bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second);
bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second, float* distance);
bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second, float* distance);
bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second, float* distance);

mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source);
float get_text_array_distance(int size, mtree_text_array* entries[size], float distances[size][size], int i, int j);

float simple_text_array_distance(mtree_text_array* first, mtree_text_array* second);
float weighted_text_array_distance(mtree_text_array* first, mtree_text_array* second);

float PCC(mtree_text_array* first, mtree_text_array* second);
float Cosine(mtree_text_array* first, mtree_text_array* second);
float Jaccard(mtree_text_array* first, mtree_text_array* second);
float ExtendedJaccard(mtree_text_array* first, mtree_text_array* second);
float TMJ(mtree_text_array* first, mtree_text_array* second);
float MSD(mtree_text_array* first, mtree_text_array* second);
float RA(mtree_text_array* first, mtree_text_array* second);

float Euclidean(mtree_text_array* first, mtree_text_array* second);
float Hamming(mtree_text_array* first, mtree_text_array* second);
float Manhatan(mtree_text_array* first, mtree_text_array* second);
float SimED(mtree_text_array* first, mtree_text_array* second);

float notCoTagsDistance(mtree_text_array* first, mtree_text_array* second);

float mtree_text_array_distance_internal(mtree_text_array* first, mtree_text_array* second);

#endif
