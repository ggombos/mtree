/*
 * contrib/mtree_gist/mtree_utils_var.h
 */
#ifndef __MTREE_UTILS_VAR_H__
#define __MTREE_UTILS_VAR_H__

#include "access/gist.h"
#include "mtree_gist.h"
#include "mb/pg_wchar.h"

/* Variable length key */
typedef struct
{
  int coveringRadius;
  int parentDistance;
  char vl_len_[4];                    /* Do not touch this field directly! */
  char vl_dat[FLEXIBLE_ARRAY_MEMBER]; /* Data content is here */
} GMT_VARKEY;

/* Better readable key */
typedef struct
{
  GMT_VARKEY* lower;
  GMT_VARKEY* upper;
} GMT_VARKEY_R;

/* Type description */
typedef struct
{
  /* Attributes */

  enum gmtree_type t; /* data type */
  int32 eml;          /* cached pg_database_encoding_max_length (0: undefined) */
  bool trnc;          /* truncate (=compress) key */

  /* Methods */

  bool (*f_gt)(const void*, const void*, Oid, FmgrInfo*);   /* greater than */
  bool (*f_ge)(const void*, const void*, Oid, FmgrInfo*);   /* greater equal */
  bool (*f_eq)(const void*, const void*, Oid, FmgrInfo*);   /* equal */
  bool (*f_le)(const void*, const void*, Oid, FmgrInfo*);   /* less equal */
  bool (*f_lt)(const void*, const void*, Oid, FmgrInfo*);   /* less than */
  int32 (*f_cmp)(const void*, const void*, Oid, FmgrInfo*); /* compare */
  GMT_VARKEY* (*f_l2n)(GMT_VARKEY*, FmgrInfo* flinfo);      /* convert leaf to node */
} gmtree_vinfo;

extern GMT_VARKEY_R gmt_var_key_readable(const GMT_VARKEY* k);

extern GMT_VARKEY* gmt_var_key_copy(const GMT_VARKEY_R* u);

extern GISTENTRY* gmt_var_compress(GISTENTRY* entry, const gmtree_vinfo* tinfo);

extern GMT_VARKEY* gmt_var_union(const GistEntryVector* entryvec, int32* size, Oid collation, const gmtree_vinfo* tinfo, FmgrInfo* flinfo);

extern bool gmt_var_same(Datum d1, Datum d2, Oid collation, const gmtree_vinfo* tinfo, FmgrInfo* flinfo);

extern float* gmt_var_penalty(float* res, const GISTENTRY* o, const GISTENTRY* n, Oid collation, const gmtree_vinfo* tinfo, FmgrInfo* flinfo);

extern bool gmt_var_consistent(GMT_VARKEY_R* key, const void* query, StrategyNumber strategy, Oid collation, bool is_leaf, const gmtree_vinfo* tinfo, FmgrInfo* flinfo);

extern GIST_SPLITVEC* gmt_var_picksplit(const GistEntryVector* entryvec, GIST_SPLITVEC* v, Oid collation, const gmtree_vinfo* tinfo, FmgrInfo* flinfo);

extern void gmt_var_bin_union(Datum* u, GMT_VARKEY* e, Oid collation, const gmtree_vinfo* tinfo, FmgrInfo* flinfo);

#endif
