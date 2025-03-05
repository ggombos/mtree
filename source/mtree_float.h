/*
 * contrib/mtree_gist/mtree_float.h
 */

#ifndef __MTREE_FLOAT_H__
#define __MTREE_FLOAT_H__

#include <math.h>
#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

#define MTREE_FLOAT_SIZE sizeof(mtree_float)
#define DatumGetMtreeFloat(x) ((mtree_float*)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_FLOAT_P(x) DatumGetMtreeFloat(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_FLOAT_P(x) PG_RETURN_POINTER(x)

typedef struct {
	float parentDistance;
	float coveringRadius;
	int level;
	float data;
} __attribute__((packed, aligned(1))) mtree_float;

#endif
