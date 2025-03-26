/*
 * contrib/mtree_gist/mtree_util.h
 */

#ifndef __MTREE_UTIL_H__
#define __MTREE_UTIL_H__

#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

double string_distance(const char*, const char*);
void init_distances(const int, double*);
double overlap_area(double, double, double);

unsigned char get_array_length(const char*, const size_t);

#endif
