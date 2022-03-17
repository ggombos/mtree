/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__
// #include "access/reloptions.h"

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


/* TODO: Some of these can be simplified */
#define SameStrategyNumber      1 //  =
#define OverlapStrategyNumber   2 // #&#
#define ContainsStrategyNumber  3 // #>#
#define ContainedStrategyNumber 4 // #<#
#define MAX2(a, b) ((a) > (b) ? (a) : (b))

#endif
