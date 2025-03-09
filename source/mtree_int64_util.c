/*
 * contrib/mtree_gist/mtree_int64_util.c
 */

#include "mtree_int64_util.h"

long long mtree_int64_outer_distance(mtree_int64* first, mtree_int64* second)
{
	long long distance = llabs(first->data - second->data);
	long long outer_distance = distance - first->coveringRadius - second->coveringRadius;

	if (outer_distance < 0.0) {
		outer_distance = 0.0;
	}

	return outer_distance;
}

long long mtree_int64_full_distance(mtree_int64* first, mtree_int64* second)
{
	return llabs(first->data - second->data);
}

bool mtree_int64_equals(mtree_int64* first, mtree_int64* second)
{
	return first->data == second->data;
}

bool mtree_int64_overlap_distance(mtree_int64* first, mtree_int64* second)
{
	long long full_distance = mtree_int64_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_int64_contains_distance(mtree_int64* first, mtree_int64* second)
{
	long long full_distance = mtree_int64_full_distance(first, second);
	return full_distance + second->coveringRadius < first->coveringRadius;
}

bool mtree_int64_contained_distance(mtree_int64* first, mtree_int64* second)
{
	return mtree_int64_contains_distance(second, first);
}

mtree_int64* mtree_int64_deep_copy(mtree_int64* source)
{
	mtree_int64* destination = (mtree_int64*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

long long get_int64_distance(int size, mtree_int64* entries[size], long long distances[size][size], int i, int j)
{
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_int64_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}
