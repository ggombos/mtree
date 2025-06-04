/*
 * contrib/mtree_gist/mtree_int32_util.c
 */

#include "mtree_int32_util.h"

double mtree_int32_outer_distance(mtree_int32* first, mtree_int32* second)
{
	double distance = abs(first->data - second->data);
	double outer_distance = distance - first->coveringRadius - second->coveringRadius;

	if (outer_distance < 0.0) {
		outer_distance = 0.0;
	}

	return outer_distance;
}

double mtree_int32_full_distance(mtree_int32* first, mtree_int32* second)
{
	return abs(first->data - second->data);
}

bool mtree_int32_equals(mtree_int32* first, mtree_int32* second)
{
	return first->data == second->data;
}

bool mtree_int32_overlap_distance(mtree_int32* first, mtree_int32* second)
{
	double full_distance = mtree_int32_full_distance(first, second);
	return full_distance - (first->coveringRadius + second->coveringRadius) < 0;
}

bool mtree_int32_contains_distance(mtree_int32* first, mtree_int32* second)
{
	double full_distance = mtree_int32_full_distance(first, second);
	return full_distance + second->coveringRadius < first->coveringRadius;
}

bool mtree_int32_contained_distance(mtree_int32* first, mtree_int32* second)
{
	return mtree_int32_contains_distance(second, first);
}

mtree_int32* mtree_int32_deep_copy(mtree_int32* source)
{
	mtree_int32* destination = (mtree_int32*)palloc(VARSIZE_ANY(source));
	memcpy(destination, source, VARSIZE_ANY(source));
	return destination;
}

double get_int32_distance(int size, mtree_int32* entries[size], double distances[size][size], int i, int j)
{
	if (distances[i][j] == -1) {
		distances[i][j] = mtree_int32_full_distance(entries[i], entries[j]);
	}
	return distances[i][j];
}
