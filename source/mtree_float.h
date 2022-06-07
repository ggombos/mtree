/*
 * contrib/mtree_gist/mtree_float.h
 */

#ifndef __MTREE_FLOAT_H__
#define __MTREE_FLOAT_H__

#include "postgres.h"
#include "access/gist.h"

#include <math.h>

#include "mtree_gist.h"

typedef struct {
	int parentDistance;
	int coveringRadius;
	float data;
} mtree_float;

#define MTREE_FLOAT_SIZE (2 * sizeof(int) + sizeof(float))
#define DatumGetMtreeFloat(x) ((mtree_float*)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_FLOAT_P(x) DatumGetMtreeFloat(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_FLOAT_P(x) PG_RETURN_POINTER(x)

#endif
