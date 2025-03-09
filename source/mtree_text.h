/*
 * contrib/mtree_gist/mtree_text.h
 */

#ifndef __MTREE_TEXT_H__
#define __MTREE_TEXT_H__

#include "mtree_gist.h"

// #define MTREE_TEXT_SIZE (3 * sizeof(int)) // 12 bytes
#define MTREE_TEXT_SIZE			  sizeof(mtree_text)  // 12 bytes
#define DatumGetMtreeText(x)	  ((mtree_text *)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_TEXT_P(x) DatumGetMtreeText(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_TEXT_P(x) PG_RETURN_POINTER(x)

typedef struct {
	int parentDistance;
	int coveringRadius;
	int level;
	char vl_length[4];
	char vl_data[FLEXIBLE_ARRAY_MEMBER];
} __attribute__((packed, aligned(1))) mtree_text;

#endif
