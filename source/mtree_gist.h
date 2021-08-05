/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef enum {
  First,
  Best
} UnionStrategy;

typedef enum {
  Random,
  FirstTwo,
  MaxDistanceFromFirst,
  MaxDistancePair,
  SamplingMinCoveringSum,
  SamplingMinCoveringMax,
  SamplingMinOverlapArea,
  SamplingMinAreaSum
} PicksplitStrategy;

#endif
