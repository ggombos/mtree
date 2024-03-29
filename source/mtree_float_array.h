/*
 * contrib/mtree_gist/mtree_float_array.h
 */

#ifndef __MTREE_FLOAT_ARRAY_H__
#define __MTREE_FLOAT_ARRAY_H__

#include "postgres.h"
#include "access/gist.h"

#include <math.h>

#include "mtree_gist.h"

typedef struct {
	int parentDistance;
	int coveringRadius;
	unsigned char arrayLength;
	float data[FLEXIBLE_ARRAY_MEMBER];
} mtree_float_array;

#define MTREE_FLOAT_ARRAY_SIZE (2 * sizeof(int) + sizeof(unsigned char))
#define DatumGetMtreeInt8Array(x) ((mtree_float_array*) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_FLOAT_ARRAY_P(x) DatumGetMtreeInt8Array(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_FLOAT_ARRAY_P(x) PG_RETURN_POINTER(x)

#endif
