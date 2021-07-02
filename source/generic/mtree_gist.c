/*
 * contrib/mtree_gist/mtree_gist.c
 */

#include "mtree_gist.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(gmt_decompress);
PG_FUNCTION_INFO_V1(gmtreekey_in);
PG_FUNCTION_INFO_V1(gmtreekey_out);

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

Datum gmt_decompress(PG_FUNCTION_ARGS)
{
  PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}
