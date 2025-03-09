/*
 * contrib/mtree_gist/mtree_int64.h
 */

#ifndef __MTREE_INT64_H__
#define __MTREE_INT64_H__

#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

#define MTREE_INT64_SIZE		   sizeof(mtree_int64)
#define DatumGetMtreeInt8(x)	   ((mtree_int64 *)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_INT64_P(x) DatumGetMtreeInt8(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_INT64_P(x) PG_RETURN_POINTER(x)

typedef struct {
	int level;
	long long parentDistance;
	long long coveringRadius;
	long long data;
} __attribute__((packed, aligned(1))) mtree_int64;

#endif
