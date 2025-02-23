/*
 * contrib/mtree_gist/mtree_int8.h
 */

#ifndef __MTREE_INT8_H__
#define __MTREE_INT8_H__

#include "postgres.h"
#include "access/gist.h"
#include "mtree_gist.h"

//#define MTREE_INT8_SIZE (2 * sizeof(int) + sizeof(long long int)) // 16 bytes
#define MTREE_INT8_SIZE sizeof(mtree_int8) // 16 bytes
#define DatumGetMtreeInt8(x) ((mtree_int8 *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_INT8_P(x) DatumGetMtreeInt8(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_INT8_P(x) PG_RETURN_POINTER(x)

typedef struct {
	int parentDistance;
	int coveringRadius;
	long long int data;
} __attribute__((packed, aligned(1))) mtree_int8;

#endif
