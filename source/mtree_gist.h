/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__

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
