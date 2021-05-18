/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__

#include "access/nbtree.h"
#include "fmgr.h"

/* Indexed types */
enum gmtree_type
{
  gmt_t_int2,
  gmt_t_text
};

#endif
