/*
 * contrib/mtree_gist/mtree_text_array.h
 */

#ifndef __MTREE_TEXT_ARRAY_H__
#define __MTREE_TEXT_ARRAY_H__

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

#define MTREE_TEXT_ARRAY_MAX_STRINGLENGTH 100

typedef struct {
	int parentDistance;
	int coveringRadius;
	unsigned char arrayLength;
	char data[FLEXIBLE_ARRAY_MEMBER][MTREE_TEXT_ARRAY_MAX_STRINGLENGTH];
} mtree_text_array;

#define MTREE_TEXT_ARRAY_DISTANCE_FUNCTION_COUNTER 2

static char mtree_text_array_distance_functions[MTREE_TEXT_ARRAY_DISTANCE_FUNCTION_COUNTER][64] = {
   "simple_text_array_distance",
   "weighted_text_array_distance"
};

#define MTREE_TEXT_ARRAY_SIZE (2 * sizeof(int) + sizeof(unsigned char))
#define DatumGetMtreeTextArray(x) ((mtree_text_array *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_TEXT_ARRAY_P(x) DatumGetMtreeTextArray(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_TEXT_ARRAY_P(x) PG_RETURN_POINTER(x)

#endif
