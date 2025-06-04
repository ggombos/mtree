/*
 * contrib/mtree_gist/mtree_int32.h
 */

#ifndef __MTREE_INT32_H__
#define __MTREE_INT32_H__

#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

#define MTREE_INT32_SIZE		   sizeof(mtree_int32)
#define DatumGetMtreeInt32(x)	   ((mtree_int32 *)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_INT32_P(x) DatumGetMtreeInt32(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_INT32_P(x) PG_RETURN_POINTER(x)

typedef struct {
	int level;
	double parentDistance;
	double coveringRadius;
	int data;
} __attribute__((packed, aligned(1))) mtree_int32;

#endif
