/*
 * contrib/mtree_gist/mtree_int8_array_util.c
 */

#include "mtree_int8_array_util.h"

int mtree_int8_array_distance_internal(mtree_int8_array* first, mtree_int8_array* second) {
  // TODO: Implement distance
  // Hasonlítsunk össze minden elemet minden elemmel és adjuk vissza, hogy hány helyen
  // egyeznek ezek a tömbök.
  // UPDATE: Csak pozíciónként! Elsőt az elsővel, másodikat a másodikkal!
  // INT8:
  // - SZUMMA: (f1 - f2) ^ 2 / f1 + f2
  // - Kullback-Leibler távolság (ha hisztogrammok a tömbök) : SZUMMA: (f1 - f2) * ln(f1 / f2)
  // - Hamming-távolság
  // Jellegzetesség-leírók: pontfelhőket tárolunk
  // TEXT:
  // - Pozíciónként, hány helyen egyeznek
  return 0;
}

bool mtree_int8_array_equals(mtree_int8_array* first, mtree_int8_array* second) {
  if (first->arrayLength != second->arrayLength) {
    return false;
  }

  for (unsigned char i = 0; i < first->arrayLength; ++i) {
    if (first->data[i] != second->data[i]) {
      return false;
    }
  }

  return true;
}

bool mtree_int8_array_overlap_distance(mtree_int8_array* first, mtree_int8_array* second, int* distance) {
  // TODO: Implement overlap distance
  return false;
}

bool mtree_int8_array_contains_distance(mtree_int8_array* first, mtree_int8_array* second, int* distance) {
  // TODO: Implement contains distance
  return false;
}

bool mtree_int8_array_contained_distance(mtree_int8_array* first, mtree_int8_array* second, int* distance) {
  return mtree_int8_array_contains_distance(second, first, distance);
}

mtree_int8_array* mtree_int8_array_deep_copy(mtree_int8_array *source) {
  mtree_int8_array* destination = (mtree_int8_array*) palloc(VARSIZE_ANY(source));
  memcpy(destination, source, VARSIZE_ANY(source));
  return destination;
}

int get_int8_array_distance(int size, mtree_int8_array* entries[size], int distances[size][size], int i, int j) {
  if (distances[i][j] == -1) {
    distances[i][j] = mtree_int8_array_distance_internal(entries[i], entries[j]);
  }
  return distances[i][j];
}
