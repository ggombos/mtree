/*
 * contrib/mtree_gist/mtree_int8_array.h
 */

#ifndef __MTREE_INT8_ARRAY_H__
#define __MTREE_INT8_ARRAY_H__

#include <math.h>
#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

//#define MTREE_INT8_ARRAY_SIZE (2 * sizeof(int) + sizeof(unsigned char)) // 9 bytes
#define MTREE_INT8_ARRAY_SIZE sizeof(mtree_int8_array) // 9 vytes
#define DatumGetMtreeInt8Array(x) ((mtree_int8_array*) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_INT8_ARRAY_P(x) DatumGetMtreeInt8Array(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_INT8_ARRAY_P(x) PG_RETURN_POINTER(x)

typedef struct {
	int parentDistance;
	long long coveringRadius;
	unsigned char arrayLength;
	long long data[FLEXIBLE_ARRAY_MEMBER];
} __attribute__((packed, aligned(1))) mtree_int8_array;

#endif
