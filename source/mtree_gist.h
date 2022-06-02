/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__

#include "postgres.h"
#include "c.h"
#include "access/reloptions.h"

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

typedef struct {
	int32 vl_len_;
	PicksplitStrategy picksplitstrategy;
	int distancestrategy;
} MtreeOptionsStruct;

static relopt_enum_elt_def PicksplitStrategyValues[] = {
	{"Random", Random},
	{"FirstTwo", FirstTwo},
	{"MaxDistanceFromFirst", MaxDistanceFromFirst},
	{"MaxDistancePair", MaxDistancePair},
	{"SamplingMinCoveringSum", SamplingMinCoveringSum},
	{"SamplingMinCoveringMax", SamplingMinCoveringMax},
	{"SamplingMinOverlapArea", SamplingMinOverlapArea},
	{"SamplingMinAreaSum", SamplingMinAreaSum},
	{(const char*)NULL}
};

/* TODO: Some of these can be simplified */
#define SameStrategyNumber      1 //  =
#define OverlapStrategyNumber   2 // #&#
#define ContainsStrategyNumber  3 // #>#
#define ContainedStrategyNumber 4 // #<#
#define MAX2(a, b) ((a) > (b) ? (a) : (b))

#endif
