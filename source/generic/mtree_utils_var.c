/*
 * contrib/mtree_gist/mtree_utils_var.c
 */
#include <float.h>
#include <limits.h>
#include <math.h>

#include "postgres.h"

#include "utils/builtins.h"
#include "utils/pg_locale.h"
#include "utils/rel.h"

#include "mtree_gist.h"
#include "mtree_utils_var.h"

/* Used for key sorting */
typedef struct
{
  int i;
  GMT_VARKEY *t;
} Vsrt;

typedef struct
{
  const gmtree_vinfo *tinfo;
  Oid collation;
  FmgrInfo *flinfo;
} gmt_vsrt_arg;

PG_FUNCTION_INFO_V1(gmt_var_decompress);
PG_FUNCTION_INFO_V1(gmt_var_fetch);

Datum gmt_var_decompress(PG_FUNCTION_ARGS)
{
  GISTENTRY *entry = (GISTENTRY *)PG_GETARG_POINTER(0);
  GMT_VARKEY *key = (GMT_VARKEY *)PG_DETOAST_DATUM(entry->key);

  if (key != (GMT_VARKEY *)DatumGetPointer(entry->key))
  {
    GISTENTRY *retval = (GISTENTRY *)palloc(sizeof(GISTENTRY));

    gistentryinit(*retval, PointerGetDatum(key), entry->rel, entry->page, entry->offset, false);

    PG_RETURN_POINTER(retval);
  }

  PG_RETURN_POINTER(entry);
}

/* Returns a better readable representation of variable key ( sets pointer ) */
GMT_VARKEY_R gmt_var_key_readable(const GMT_VARKEY *k)
{
  GMT_VARKEY_R r;
  r.lower = (bytea *)&(((char *)k)[VARHDRSZ]);
  if (VARSIZE(k) > (VARHDRSZ + (VARSIZE(r.lower))))
  {
    r.upper = (bytea *)&(((char *)k)[VARHDRSZ + INTALIGN(VARSIZE(r.lower))]);
  }
  else
  {
    r.upper = r.lower;
  }
  return r;
}

Datum gmt_var_fetch(PG_FUNCTION_ARGS)
{
  GISTENTRY *entry = (GISTENTRY *)PG_GETARG_POINTER(0);
  GMT_VARKEY *key = (GMT_VARKEY *)PG_DETOAST_DATUM(entry->key);
  GMT_VARKEY_R r = gmt_var_key_readable(key);
  GISTENTRY *retval;

  retval = palloc(sizeof(GISTENTRY));
  gistentryinit(*retval, PointerGetDatum(r.lower), entry->rel, entry->page, entry->offset, true);

  PG_RETURN_POINTER(retval);
}
