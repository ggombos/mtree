/*
 * contrib/mtree_gist/mtree_gist.c
 */
#include "postgres.h"

#include "utils/builtins.h"

#include "mtree_gist.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(gmt_decompress);
PG_FUNCTION_INFO_V1(gmtreekey_in);
PG_FUNCTION_INFO_V1(gmtreekey_out);

/**************************************************
 * In/Out for keys
 **************************************************/

Datum gmtreekey_in(PG_FUNCTION_ARGS)
{
  ereport(ERROR,
          (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
           errmsg("<datatype>key_in() not implemented")));

  PG_RETURN_POINTER(NULL);
}

Datum gmtreekey_out(PG_FUNCTION_ARGS)
{
  ereport(ERROR,
          (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
           errmsg("<datatype>key_out() not implemented")));

  PG_RETURN_POINTER(NULL);
}

/* GiST DeCompress methods do not do anything. */
Datum gmt_decompress(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}
