/*
 * contrib/mtree_gist/mtree_float_array.h
 */

#ifndef __MTREE_FLOAT_ARRAY_H__
#define __MTREE_FLOAT_ARRAY_H__

#include <math.h>
#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

//#define MTREE_FLOAT_ARRAY_SIZE (2 * sizeof(float) + sizeof(unsigned char) + sizeof(int)) // 13 bytes
#define MTREE_FLOAT_ARRAY_SIZE sizeof(mtree_float_array) // 13 bytes
#define DatumGetMtreeInt8Array(x) ((mtree_float_array*) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_FLOAT_ARRAY_P(x) DatumGetMtreeInt8Array(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_FLOAT_ARRAY_P(x) PG_RETURN_POINTER(x)

typedef struct {
	float parentDistance;
	float coveringRadius;
	unsigned char arrayLength;
	int level;
	float data[FLEXIBLE_ARRAY_MEMBER];
} __attribute__((packed, aligned(1))) mtree_float_array;

#endif
