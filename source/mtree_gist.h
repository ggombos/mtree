/*
 * contrib/mtree_gist/mtree_gist.h
 */
#ifndef __MTREE_GIST_H__
#define __MTREE_GIST_H__

#include "postgres.h"
#include "access/reloptions.h"

/*
 * Union strategies
 */
typedef enum
{
	/* Choose the first entry. */
	First,
	/* Choose the entry with the minimal maximum distance from the others. */
	MinMaxDistance
}
MtreeUnionStrategy;

/*
 * PickSplit strategies
 */
typedef enum
{
	/* Choose two nodes randomly. */
	Random,
	/* Choose the first two nodes. */
	FirstTwo,
	/*  */
	MaxDistanceFromFirst,
	/*  */
	MaxDistancePair,
	/*  */
	SamplingMinCoveringSum,
	/*  */
	SamplingMinCoveringMax,
	/*  */
	SamplingMinOverlapArea,
	/*  */
	SamplingMinAreaSum
}
MtreePickSplitStrategy;

/*
 * Operator class options
 */
typedef struct
{
	/* varlena header (do not touch directly!) */
	int32	vl_len_;
	/* PickSplit strategy */
	MtreePickSplitStrategy picksplit_strategy;
	/* Union strategy */
	MtreeUnionStrategy union_strategy;
}
MtreeOptions;

/*
 * String representation of MtreeUnionStrategy values for
 * operator class option support.
 */
static relopt_enum_elt_def mtreeUnionStrategyValues[] =
{
	{"First",			First},
	{"MinMaxDistance",	MinMaxDistance},
	{(const char *) NULL}
};

/*
 * String representation of MtreePickSplitStrategy values for
 * operator class option support.
 */
static relopt_enum_elt_def mtreePickSplitStrategyValues[] =
{
	{"Random",					Random},
	{"FirstTwo",				FirstTwo},
	{"MaxDistanceFromFirst",	MaxDistanceFromFirst},
	{"MaxDistancePair",			MaxDistancePair},
	{"SamplingMinCoveringSum",	SamplingMinCoveringSum},
	{"SamplingMinCoveringMax",	SamplingMinCoveringMax},
	{"SamplingMinOverlapArea",	SamplingMinOverlapArea},
	{"SamplingMinAreaSum",		SamplingMinAreaSum},
	{(const char*) NULL}
};

/*
 * GiST Strategy Numbers
 */
#define	GIST_SN_STRICTLY_LEFT_OF			1
#define	GIST_SN_DOES_NOT_EXTEND_TO_RIGHT_OF	2
#define	GIST_SN_OVERLAPS					3
#define	GIST_SN_DOES_NOT_EXTEND_TO_LEFT_OF	4
#define	GIST_SN_STRICTLY_RIGHT_OF			5
#define	GIST_SN_SAME						6
#define	GIST_SN_CONTAINS					7
#define	GIST_SN_CONTAINED_BY				8
#define	GIST_SN_DOES_NOT_EXTEND_ABOVE		9
#define	GIST_SN_STRICTLY_BELOW				10
#define	GIST_SN_STRICTLY_ABOVE				11
#define	GIST_SN_DOES_NOT_EXTEND_BELOW		12

/*
 * Useful macros
 */
#define MAX_2(x, y)					(((x) > (y)) ? (x) : (y))
#define MIN_2(x, y)					(((x) < (y)) ? (x) : (y))

#endif
