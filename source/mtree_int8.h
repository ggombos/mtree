/*
 * contrib/mtree_gist/mtree_int8.h
 */

#ifndef __MTREE_INT8_H__
#define __MTREE_INT8_H__

#include "postgres.h"
#include "c.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"
#include "utils/elog.h"
#include "utils/palloc.h"
#include "access/genam.h"
#include "access/gist.h"
#include "access/gist_private.h"
#include "access/skey.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <utils/array.h>
#include <utils/lsyscache.h>

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
