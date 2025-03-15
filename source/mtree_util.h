/*
 * contrib/mtree_gist/mtree_util.h
 */

#ifndef __MTREE_UTIL_H__
#define __MTREE_UTIL_H__

#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int string_distance(const char*, const char*);
void init_distances_long_long(const int, long long*);
void init_distances_float(const int, float*);
void init_distances_int(const int, int*);
double overlap_area(const long long, const long long, const long long);
double overlap_area_float(float, float, float);

unsigned char get_array_length(const char*, const size_t);

#endif
