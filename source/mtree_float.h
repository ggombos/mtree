/*
 * contrib/mtree_gist/mtree_float.h
 */

#ifndef __MTREE_FLOAT_H__
#define __MTREE_FLOAT_H__

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
  float data;
} mtree_float;

#define MTREE_FLOAT_SIZE (2 * sizeof(int) + sizeof(float))
#define DatumGetMtreeFloat(x) ((mtree_float *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_FLOAT_P(x) DatumGetMtreeFloat(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_FLOAT_P(x) PG_RETURN_POINTER(x)

#endif
