/*
 * contrib/mtree_gist/mtree_text_array_util.h
 */

#ifndef __MTREE_TEXT_ARRAY_UTIL_H__
#define __MTREE_TEXT_ARRAY_UTIL_H__

#include "mtree_text_array.h"

double mtree_text_array_outer_distance(mtree_text_array* first, mtree_text_array* second);
double mtree_text_array_full_distance(mtree_text_array* first, mtree_text_array* second);

bool mtree_text_array_equals(mtree_text_array* first, mtree_text_array* second);
bool mtree_text_array_overlap_distance(mtree_text_array* first, mtree_text_array* second);
bool mtree_text_array_contains_distance(mtree_text_array* first, mtree_text_array* second);
bool mtree_text_array_contained_distance(mtree_text_array* first, mtree_text_array* second);

mtree_text_array* mtree_text_array_deep_copy(mtree_text_array* source);
double get_text_array_distance(int size, mtree_text_array* entries[size], double distances[size][size], int i, int j);

double simple_text_array_distance(mtree_text_array* first, mtree_text_array* second);
double weighted_text_array_distance(mtree_text_array* first, mtree_text_array* second);

double PCC(mtree_text_array* first, mtree_text_array* second);
double Cosine(mtree_text_array* first, mtree_text_array* second);
double Jaccard(mtree_text_array* first, mtree_text_array* second);
double ExtendedJaccard(mtree_text_array* first, mtree_text_array* second);
double TMJ(mtree_text_array* first, mtree_text_array* second);
double MSD(mtree_text_array* first, mtree_text_array* second);
double RA(mtree_text_array* first, mtree_text_array* second);

double Euclidean(mtree_text_array* first, mtree_text_array* second);
double Hamming(mtree_text_array* first, mtree_text_array* second);
double Manhatan(mtree_text_array* first, mtree_text_array* second);
double SimED(mtree_text_array* first, mtree_text_array* second);

double notCoTagsDistance(mtree_text_array* first, mtree_text_array* second);

#endif
