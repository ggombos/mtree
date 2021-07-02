/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__

#include "postgres.h"
#include "access/nbtree.h"

/* Indexed types */
enum gmtree_type
{
  gmt_t_int2,
  gmt_t_text
};

#endif
