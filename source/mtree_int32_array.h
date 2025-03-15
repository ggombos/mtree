/*
 * contrib/mtree_gist/mtree_int32_array.h
 */

#ifndef __MTREE_INT32_ARRAY_H__
#define __MTREE_INT32_ARRAY_H__

#include <math.h>

#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

#define MTREE_INT32_ARRAY_SIZE			 sizeof(mtree_int32_array)
#define DatumGetMtreeInt32Array(x)		 ((mtree_int32_array*)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_INT32_ARRAY_P(x) DatumGetMtreeInt32Array(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_INT32_ARRAY_P(x) PG_RETURN_POINTER(x)

typedef struct {
	int level;
	long long parentDistance;
	long long coveringRadius;
	unsigned char arrayLength;
	int data[FLEXIBLE_ARRAY_MEMBER];
} __attribute__((packed, aligned(1))) mtree_int32_array;

#endif
