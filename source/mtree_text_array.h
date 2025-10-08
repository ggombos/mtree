/*
 * contrib/mtree_gist/mtree_text_array.h
 */

#ifndef __MTREE_TEXT_ARRAY_H__
#define __MTREE_TEXT_ARRAY_H__

#include "mtree_gist.h"

#define MTREE_TEXT_ARRAY_MAX_STRINGLENGTH 2048
#define MTREE_TEXT_ARRAY_SIZE			  sizeof(mtree_text_array)
#define DatumGetMtreeTextArray(x)		  ((mtree_text_array *)PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_TEXT_ARRAY_P(x)	  DatumGetMtreeTextArray(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_TEXT_ARRAY_P(x)	  PG_RETURN_POINTER(x)

typedef struct {
	double parentDistance;
	double coveringRadius;
	int level;
	unsigned char arrayLength;
	char data[FLEXIBLE_ARRAY_MEMBER][MTREE_TEXT_ARRAY_MAX_STRINGLENGTH];
} __attribute__((packed, aligned(1))) mtree_text_array;

#endif
