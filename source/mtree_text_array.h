/*
 * contrib/mtree_gist/mtree_text_array.h
 */

#ifndef __MTREE_TEXT_ARRAY_H__
#define __MTREE_TEXT_ARRAY_H__

#include "postgres.h"
#include "access/gist.h"

#include "mtree_gist.h"

#define MTREE_TEXT_ARRAY_MAX_STRINGLENGTH 128
#define MTREE_TEXT_ARRAY_SIZE (2 * sizeof(float) + sizeof(unsigned char))
#define DatumGetMtreeTextArray(x) ((mtree_text_array *) PG_DETOAST_DATUM(x))
#define PG_GETARG_MTREE_TEXT_ARRAY_P(x) DatumGetMtreeTextArray(PG_GETARG_DATUM(x))
#define PG_RETURN_MTREE_TEXT_ARRAY_P(x) PG_RETURN_POINTER(x)

typedef struct
{
	float parentDistance;
	float coveringRadius;
	unsigned char arrayLength;
	char data[FLEXIBLE_ARRAY_MEMBER][MTREE_TEXT_ARRAY_MAX_STRINGLENGTH];
}
mtree_text_array;

#endif
