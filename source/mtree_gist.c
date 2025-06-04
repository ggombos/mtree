/*
 * contrib/mtree_gist/mtree_gist.c
 */
#include "mtree_gist.h"

#include "postgres.h"
#include "fmgr.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(mtree_options);

/*
 * String representation of MtreePickSplitStrategy values for
 * operator class option support.
 */
relopt_enum_elt_def mtreePickSplitStrategyValues[] =
{
	{"Random",					Random},
	{"FirstTwo",				FirstTwo},
	{"MaxDistanceFromFirst",	MaxDistanceFromFirst},
	{"MaxDistancePair",			MaxDistancePair},
	{"SamplingMinCoveringSum",	SamplingMinCoveringSum},
	{"SamplingMinCoveringMax",	SamplingMinCoveringMax},
	{"SamplingMinOverlapArea",	SamplingMinOverlapArea},
	{"SamplingMinAreaSum",		SamplingMinAreaSum},
	{"GuttmanPolyTime", 		GuttmanPolyTime},
	{(const char*) NULL}
};

/*
 * String representation of MtreeUnionStrategy values for
 * operator class option support.
 */
relopt_enum_elt_def mtreeUnionStrategyValues[] =
{
	{"First",			First},
	{"MinMaxDistance",	MinMaxDistance},
	{(const char *) NULL}
};

Datum mtree_options(PG_FUNCTION_ARGS)
{
	local_relopts *relopts = (local_relopts *) PG_GETARG_POINTER(0);

	init_local_reloptions(relopts, sizeof(MtreeOptions));

	add_local_enum_reloption(
		relopts,
		"picksplit_strategy",
		"PickSplit strategies for the M-tree index implementation",
		mtreePickSplitStrategyValues,
		SamplingMinOverlapArea,
		"Valid values are: \"Random\", \"FirstTwo\", \"MaxDistanceFromFirst\", \"MaxDistancePair\", \"SamplingMinCoveringSum\", \"SamplingMinCoveringMax\", \"SamplingMinOverlapArea\" and \"SamplingMinAreaSum\".",
		offsetof(MtreeOptions, picksplit_strategy));

	add_local_enum_reloption(
		relopts,
		"union_strategy",
		"Union strategies for the M-tree index implementation",
		mtreeUnionStrategyValues,
		MinMaxDistance,
		"Valid values are: \"First\" and \"MinMaxDistance\".",
		offsetof(MtreeOptions, union_strategy));

	PG_RETURN_VOID();
}
