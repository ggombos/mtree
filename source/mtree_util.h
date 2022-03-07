/*
 * contrib/mtree_gist/mtree_util.h
 */

#ifndef __MTREE_UTIL_H__
#define __MTREE_UTIL_H__

#include <math.h>
#include <regex.h>
#include <stdbool.h>
#include <string.h>

#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

int string_distance(const char*, const char*);
void init_distances(const int, int*);
double overlap_area(const int, const int, const int);

unsigned char get_array_length(const char*);

bool is_valid_string(const char*, const char*);

#endif
