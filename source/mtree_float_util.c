/*
 * contrib/mtree_gist/mtree_float_util.c
 */

#include "mtree_float_util.h"

float mtree_float_distance_internal(mtree_float* first, mtree_float* second) {
  return fabs(first->data - second->data);
}

bool mtree_float_equals(mtree_float* first, mtree_float* second) {
  return (first->data - second->data) == 0;
}

bool mtree_float_overlap_distance(mtree_float* first, mtree_float* second, float* distance) {
  return *distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_float_contains_distance(mtree_float* first, mtree_float* second, float* distance) {
  return first->coveringRadius >= *distance + second->coveringRadius;
}

bool mtree_float_contained_distance(mtree_float* first, mtree_float* second, float* distance) {
  return mtree_float_contains_distance(second, first, distance);
}

mtree_float* mtree_float_deep_copy(mtree_float *source) {
  mtree_float* destination = (mtree_float*) palloc(VARSIZE_ANY(source));
  memcpy(destination, source, VARSIZE_ANY(source));
  return destination;
}

int get_float_distance(int size, mtree_float* entries[size], int distances[size][size], int i, int j) {
  if (distances[i][j] == -1) {
    distances[i][j] = mtree_float_distance_internal(entries[i], entries[j]);
  }
  return distances[i][j];
}
