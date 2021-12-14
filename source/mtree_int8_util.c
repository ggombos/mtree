/*
 * contrib/mtree_gist/mtree_int8_util.c
 */

#include "mtree_int8_util.h"

int mtree_int8_distance_internal(mtree_int8* first, mtree_int8* second) {
  /* QUESTION: How to implement distance? */
  return abs(first->data - second->data);
}

bool mtree_int8_equals(mtree_int8* first, mtree_int8* second) {
  return (first->data - second->data) == 0;
}

bool mtree_int8_overlap_distance(mtree_int8* first, mtree_int8* second, int* distance) {
  return *distance <= first->coveringRadius + second->coveringRadius;
}

bool mtree_int8_contains_distance(mtree_int8* first, mtree_int8* second, int* distance) {
  return first->coveringRadius >= *distance + second->coveringRadius;
}

bool mtree_int8_contained_distance(mtree_int8* first, mtree_int8* second, int* distance) {
  return mtree_int8_contains_distance(second, first, distance);
}

mtree_int8* mtree_int8_deep_copy(mtree_int8 *source) {
  mtree_int8* destination = (mtree_int8*) palloc(VARSIZE_ANY(source));
  memcpy(destination, source, VARSIZE_ANY(source));
  return destination;
}

int get_int8_distance(int size, mtree_int8* entries[size], int distances[size][size], int i, int j) {
  if (distances[i][j] == -1) {
    distances[i][j] = mtree_int8_distance_internal(entries[i], entries[j]);
  }
  return distances[i][j];
}
