/*
 * contrib/mtree_gist/mtree_text.h
 */

#ifndef __MTREE_TEXT_H__
#define __MTREE_TEXT_H__

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
  char vl_length[4];
  char vl_data[FLEXIBLE_ARRAY_MEMBER];
} mtree_text;

/* TODO: Some of these can be simplified */
#define SameStrategyNumber      1 //  =
#define OverlapStrategyNumber   2 // #&#
#define ContainsStrategyNumber  3 // #>#
#define ContainedStrategyNumber 4 // #<#
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define OWNHDRSZ (3 * sizeof(int))
#define DatumGetMtreeText(x) ((mtree_text *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_TEXT_P(x) DatumGetMtreeText(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_TEXT_P(x) PG_RETURN_POINTER(x)

#endif
