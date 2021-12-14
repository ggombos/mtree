/*
 * contrib/mtree_gist/mtree_util.h
 */

#ifndef __MTREE_UTIL_H__
#define __MTREE_UTIL_H__

#include <math.h>
#include <string.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int string_distance(char* a, char* b);
void init_distances(int size, int* distances);
double overlap_area(int radiusOne, int radiusTwo, int distance);

#endif
