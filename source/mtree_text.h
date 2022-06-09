/*
 * contrib/mtree_gist/mtree_text.h
 */

#ifndef __MTREE_TEXT_H__
#define __MTREE_TEXT_H__

#include "postgres.h"
#include "access/gist.h"

#include "mtree_gist.h"

/*
 * Storage type for text data.
 */
typedef struct {
	int parentDistance;
	int coveringRadius;
	char vl_length[4];
	char vl_data[FLEXIBLE_ARRAY_MEMBER];
} mtree_text;

#define OWNHDRSZ (3 * sizeof(int))
#define DatumGetMtreeText(x) ((mtree_text *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_TEXT_P(x) DatumGetMtreeText(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_TEXT_P(x) PG_RETURN_POINTER(x)

#endif
