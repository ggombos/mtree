/*
 * contrib/mtree_gist/mtree_float_array.h
 */

#ifndef __MTREE_FLOAT_ARRAY_H__
#define __MTREE_FLOAT_ARRAY_H__

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
  unsigned char arrayLength;
  float data[FLEXIBLE_ARRAY_MEMBER];
} mtree_float_array;

#define MTREE_FLOAT_ARRAY_SIZE (2 * sizeof(int) + sizeof(unsigned char))
#define DatumGetMtreeInt8Array(x) ((mtree_float_array *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_FLOAT_ARRAY_P(x) DatumGetMtreeInt8Array(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_FLOAT_ARRAY_P(x) PG_RETURN_POINTER(x)

Datum mtree_float_array_input(PG_FUNCTION_ARGS);
Datum mtree_float_array_output(PG_FUNCTION_ARGS);
Datum mtree_float_array_consistent(PG_FUNCTION_ARGS);
Datum mtree_float_array_union(PG_FUNCTION_ARGS);
Datum mtree_float_array_same(PG_FUNCTION_ARGS);
Datum mtree_float_array_penalty(PG_FUNCTION_ARGS);
Datum mtree_float_array_picksplit(PG_FUNCTION_ARGS);
Datum mtree_float_array_compress(PG_FUNCTION_ARGS);
Datum mtree_float_array_decompress(PG_FUNCTION_ARGS);
Datum mtree_float_array_distance(PG_FUNCTION_ARGS);
Datum mtree_float_array_distance_operator(PG_FUNCTION_ARGS);
Datum mtree_float_array_overlap_operator(PG_FUNCTION_ARGS);
Datum mtree_float_array_contains_operator(PG_FUNCTION_ARGS);
Datum mtree_float_array_contained_operator(PG_FUNCTION_ARGS);

#endif
