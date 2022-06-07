/*
 * contrib/mtree_gist/mtree_int8.h
 */

#ifndef __MTREE_INT8_H__
#define __MTREE_INT8_H__

#include "postgres.h"
#include "access/gist.h"

#include "mtree_gist.h"

typedef struct {
	int parentDistance;
	int coveringRadius;
	int64 data;
} mtree_int8;

#define MTREE_INT8_SIZE (2 * sizeof(int) + sizeof(int64))
#define DatumGetMtreeInt8(x) ((mtree_int8 *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_INT8_P(x) DatumGetMtreeInt8(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_INT8_P(x) PG_RETURN_POINTER(x)

#endif
