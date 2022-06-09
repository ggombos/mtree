/* contrib/mtree_gist/mtree_gist--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mtree_gist" to load this file. \quit

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- Type independent functions
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE OR REPLACE FUNCTION mtree_options(internal)
RETURNS void
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- mtree_text
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE TYPE mtree_text;

CREATE OR REPLACE FUNCTION mtree_text_input(cstring)
RETURNS mtree_text
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_output(mtree_text)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mtree_text (
	INPUT	= mtree_text_input,
	OUTPUT	= mtree_text_output,
	INTERNALLENGTH = VARIABLE,
	STORAGE	= extended
);

CREATE OR REPLACE FUNCTION mtree_text_consistent(internal, mtree_text, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_union(internal, internal)
RETURNS mtree_text
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_same(mtree_text, mtree_text, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_distance(internal, mtree_text, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_operator_overlap(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_operator_same(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_operator_contains(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_operator_contained(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_operator_distance(mtree_text, mtree_text)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR #&# (
	LEFTARG		= mtree_text,
	RIGHTARG	= mtree_text,
	FUNCTION	= mtree_text_operator_overlap,
	COMMUTATOR	= #&#
);

CREATE OPERATOR = (
	LEFTARG		= mtree_text,
	RIGHTARG	= mtree_text,
	FUNCTION	= mtree_text_operator_same,
	COMMUTATOR	= =
);

CREATE OPERATOR #># (
	LEFTARG		= mtree_text,
	RIGHTARG	= mtree_text,
	FUNCTION	= mtree_text_operator_contains,
	COMMUTATOR	= #<#
);

CREATE OPERATOR #<# (
	LEFTARG		= mtree_text,
	RIGHTARG	= mtree_text,
	FUNCTION	= mtree_text_operator_contained,
	COMMUTATOR	= #>#
);

CREATE OPERATOR <-> (
	LEFTARG		= mtree_text,
	RIGHTARG	= mtree_text,
	FUNCTION	= mtree_text_operator_distance,
	COMMUTATOR	= <->
);

CREATE OPERATOR CLASS gist_mtree_text_ops
DEFAULT FOR TYPE mtree_text USING gist AS
	OPERATOR	3	#&#	,
	OPERATOR	6	=	,
	OPERATOR	7	#>#	,
	OPERATOR	8	#<#	,
	OPERATOR	15	<->						(mtree_text, mtree_text) FOR ORDER BY float_ops,
	FUNCTION	1	mtree_text_consistent	(internal, mtree_text, smallint, oid, internal),
	FUNCTION	2	mtree_text_union		(internal, internal),
	FUNCTION	5	mtree_text_penalty		(internal, internal, internal),
	FUNCTION	6	mtree_text_picksplit	(internal, internal),
	FUNCTION	7	mtree_text_same			(mtree_text, mtree_text, internal),
	FUNCTION	8	mtree_text_distance		(internal, mtree_text, smallint, oid, internal),
	FUNCTION	10	mtree_options			(internal);

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- _mtree_text
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE TYPE mtree_text_array;

CREATE OR REPLACE FUNCTION mtree_text_array_input(cstring)
RETURNS mtree_text_array
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_output(mtree_text_array)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mtree_text_array (
	INPUT	= mtree_text_array_input,
	OUTPUT	= mtree_text_array_output,
	INTERNALLENGTH = VARIABLE,
	STORAGE	= extended
);

CREATE OR REPLACE FUNCTION mtree_text_array_consistent(internal, mtree_text_array, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_union(internal, internal)
RETURNS mtree_text_array
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_same(mtree_text_array, mtree_text_array, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_distance(internal, mtree_text_array, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_operator_overlap(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_operator_same(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_operator_contains(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_operator_contained(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_text_array_operator_distance(mtree_text_array, mtree_text_array)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR = (
	LEFTARG		= mtree_text_array,
	RIGHTARG	= mtree_text_array,
	FUNCTION	= mtree_text_array_operator_same,
	COMMUTATOR	= =
);

CREATE OPERATOR #&# (
	LEFTARG		= mtree_text_array,
	RIGHTARG	= mtree_text_array,
	FUNCTION	= mtree_text_array_operator_overlap,
	COMMUTATOR	= #&#
);

CREATE OPERATOR #># (
	LEFTARG		= mtree_text_array,
	RIGHTARG	= mtree_text_array,
	FUNCTION	= mtree_text_array_operator_contains,
	COMMUTATOR	= #<#
);

CREATE OPERATOR #<# (
	LEFTARG		= mtree_text_array,
	RIGHTARG	= mtree_text_array,
	FUNCTION	= mtree_text_array_operator_contained,
	COMMUTATOR	= #>#
);

CREATE OPERATOR <-> (
	LEFTARG		= mtree_text_array,
	RIGHTARG	= mtree_text_array,
	FUNCTION	= mtree_text_array_operator_distance,
	COMMUTATOR	= <->
);

CREATE OPERATOR CLASS gist_mtree_text_array_ops
DEFAULT FOR TYPE mtree_text_array USING gist AS
	OPERATOR	3	=	,
	OPERATOR	6	#&#	,
	OPERATOR	7	#>#	,
	OPERATOR	8	#<#	,
	OPERATOR	15	<->							(mtree_text_array, mtree_text_array) FOR ORDER BY float_ops,
	FUNCTION	1	mtree_text_array_consistent	(internal, mtree_text_array, smallint, oid, internal),
	FUNCTION	2	mtree_text_array_union		(internal, internal),
	FUNCTION	3	mtree_text_array_compress	(internal),
	FUNCTION	4	mtree_text_array_decompress	(internal),
	FUNCTION	5	mtree_text_array_penalty	(internal, internal, internal),
	FUNCTION	6	mtree_text_array_picksplit	(internal, internal),
	FUNCTION	7	mtree_text_array_same		(mtree_text_array, mtree_text_array, internal),
	FUNCTION	8	mtree_text_array_distance	(internal, mtree_text_array, smallint, oid, internal),
	FUNCTION	10	mtree_options				(internal);

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- mtree_int8
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE TYPE mtree_int8;

CREATE OR REPLACE FUNCTION mtree_int8_input(cstring)
RETURNS mtree_int8
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_output(mtree_int8)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mtree_int8 (
	INPUT	= mtree_int8_input,
	OUTPUT	= mtree_int8_output,
	INTERNALLENGTH = VARIABLE,
	STORAGE	= extended
);

CREATE OR REPLACE FUNCTION mtree_int8_consistent(internal, mtree_int8, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_union(internal, internal)
RETURNS mtree_int8
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_same(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_distance(internal, mtree_int8, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_overlap_operator(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_contains_operator(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_contained_operator(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_distance_operator(mtree_int8, mtree_int8)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR = (
	LEFTARG		= mtree_int8,
	RIGHTARG	= mtree_int8,
	FUNCTION	= mtree_int8_same,
	COMMUTATOR	= =
);

CREATE OPERATOR #&# (
	LEFTARG		= mtree_int8,
	RIGHTARG	= mtree_int8,
	FUNCTION	= mtree_int8_overlap_operator,
	COMMUTATOR	= #&#
);

CREATE OPERATOR #<# (
	LEFTARG		= mtree_int8,
	RIGHTARG	= mtree_int8,
	FUNCTION	= mtree_int8_contained_operator,
	COMMUTATOR	= #>#
);

CREATE OPERATOR #># (
	LEFTARG		= mtree_int8,
	RIGHTARG	= mtree_int8,
	FUNCTION	= mtree_int8_contains_operator,
	COMMUTATOR	= #<#
);

CREATE OPERATOR <-> (
	LEFTARG		= mtree_int8,
	RIGHTARG	= mtree_int8,
	FUNCTION	= mtree_int8_distance_operator,
	COMMUTATOR	= <->
);

CREATE OPERATOR CLASS gist_mtree_int8_ops
DEFAULT FOR TYPE mtree_int8 USING gist AS
	OPERATOR	1	=	,
	OPERATOR	2	#&#	,
	OPERATOR	3	#>#	,
	OPERATOR	4	#<#	,
	OPERATOR	15	<->						(mtree_int8, mtree_int8) FOR ORDER BY float_ops,
	FUNCTION	1	mtree_int8_consistent	(internal, mtree_int8, smallint, oid, internal),
	FUNCTION	2	mtree_int8_union		(internal, internal),
	FUNCTION	3	mtree_int8_compress		(internal),
	FUNCTION	4	mtree_int8_decompress	(internal),
	FUNCTION	5	mtree_int8_penalty		(internal, internal, internal),
	FUNCTION	6	mtree_int8_picksplit	(internal, internal),
	FUNCTION	7	mtree_int8_same			(mtree_int8, mtree_int8),
	FUNCTION	8	mtree_int8_distance		(internal, mtree_int8, smallint, oid, internal);

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- _mtree_int8
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE TYPE mtree_int8_array;

CREATE OR REPLACE FUNCTION mtree_int8_array_input(cstring)
RETURNS mtree_int8_array
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_output(mtree_int8_array)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mtree_int8_array (
	INPUT	= mtree_int8_array_input,
	OUTPUT	= mtree_int8_array_output,
	INTERNALLENGTH = VARIABLE,
	STORAGE	= extended
);

CREATE OR REPLACE FUNCTION mtree_int8_array_consistent(internal, mtree_int8_array, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_union(internal, internal)
RETURNS mtree_int8_array
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_same(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_distance(internal, mtree_int8_array, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_overlap_operator(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_contains_operator(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_contained_operator(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_int8_array_distance_operator(mtree_int8_array, mtree_int8_array)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR = (
	LEFTARG		= mtree_int8_array,
	RIGHTARG	= mtree_int8_array,
	FUNCTION	= mtree_int8_array_same,
	COMMUTATOR	= =
);

CREATE OPERATOR #&# (
	LEFTARG		= mtree_int8_array,
	RIGHTARG	= mtree_int8_array,
	FUNCTION	= mtree_int8_array_overlap_operator,
	COMMUTATOR	= #&#
);

CREATE OPERATOR #># (
	LEFTARG		= mtree_int8_array,
	RIGHTARG	= mtree_int8_array,
	FUNCTION	= mtree_int8_array_contains_operator,
	COMMUTATOR	= #<#
);

CREATE OPERATOR #<# (
	LEFTARG		= mtree_int8_array,
	RIGHTARG	= mtree_int8_array,
	FUNCTION	= mtree_int8_array_contained_operator,
	COMMUTATOR	= #>#
);

CREATE OPERATOR <-> (
	LEFTARG		= mtree_int8_array,
	RIGHTARG	= mtree_int8_array,
	FUNCTION	= mtree_int8_array_distance_operator,
	COMMUTATOR	= <->
);

CREATE OPERATOR CLASS gist_mtree_int8_array_ops
DEFAULT FOR TYPE mtree_int8_array USING gist AS
	OPERATOR	1	=	,
	OPERATOR	2	#&#	,
	OPERATOR	3	#>#	,
	OPERATOR	4	#<#	,
	OPERATOR	15	<->							(mtree_int8_array, mtree_int8_array) FOR ORDER BY float_ops,
	FUNCTION	1	mtree_int8_array_consistent	(internal, mtree_int8_array, smallint, oid, internal),
	FUNCTION	2	mtree_int8_array_union		(internal, internal),
	FUNCTION	3	mtree_int8_array_compress	(internal),
	FUNCTION	4	mtree_int8_array_decompress	(internal),
	FUNCTION	5	mtree_int8_array_penalty	(internal, internal, internal),
	FUNCTION	6	mtree_int8_array_picksplit	(internal, internal),
	FUNCTION	7	mtree_int8_array_same		(mtree_int8_array, mtree_int8_array),
	FUNCTION	8	mtree_int8_array_distance	(internal, mtree_int8_array, smallint, oid, internal);

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- mtree_float
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE TYPE mtree_float;

CREATE OR REPLACE FUNCTION mtree_float_input(cstring)
RETURNS mtree_float
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_output(mtree_float)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mtree_float (
	INPUT	= mtree_float_input,
	OUTPUT	= mtree_float_output,
	INTERNALLENGTH = VARIABLE,
	STORAGE	= extended
);

CREATE OR REPLACE FUNCTION mtree_float_consistent(internal, mtree_float, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_union(internal, internal)
RETURNS mtree_float
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_same(mtree_float, mtree_float)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_distance(internal, mtree_float, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_overlap_operator(mtree_float, mtree_float)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_contains_operator(mtree_float, mtree_float)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_contained_operator(mtree_float, mtree_float)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_distance_operator(mtree_float, mtree_float)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR = (
	LEFTARG		= mtree_float,
	RIGHTARG	= mtree_float,
	FUNCTION	= mtree_float_same,
	COMMUTATOR	= =
);

CREATE OPERATOR #&# (
	LEFTARG		= mtree_float,
	RIGHTARG	= mtree_float,
	FUNCTION	= mtree_float_overlap_operator,
	COMMUTATOR	= #&#
);

CREATE OPERATOR #># (
	LEFTARG		= mtree_float,
	RIGHTARG	= mtree_float,
	FUNCTION	= mtree_float_contains_operator,
	COMMUTATOR	= #<#
);

CREATE OPERATOR #<# (
	LEFTARG		= mtree_float,
	RIGHTARG	= mtree_float,
	FUNCTION	= mtree_float_contained_operator,
	COMMUTATOR	= #>#
);

CREATE OPERATOR <-> (
	LEFTARG		= mtree_float,
	RIGHTARG	= mtree_float,
	FUNCTION	= mtree_float_distance_operator,
	COMMUTATOR	= <->
);

CREATE OPERATOR CLASS gist_mtree_float_ops
DEFAULT FOR TYPE mtree_float USING gist AS
	OPERATOR	1	=	,
	OPERATOR	2	#&#	,
	OPERATOR	3	#>#	,
	OPERATOR	4	#<#	,
	OPERATOR	15	<->						(mtree_float, mtree_float) FOR ORDER BY float_ops,
	FUNCTION	1	mtree_float_consistent	(internal, mtree_float, smallint, oid, internal),
	FUNCTION	2	mtree_float_union		(internal, internal),
	FUNCTION	3	mtree_float_compress	(internal),
	FUNCTION	4	mtree_float_decompress	(internal),
	FUNCTION	5	mtree_float_penalty		(internal, internal, internal),
	FUNCTION	6	mtree_float_picksplit	(internal, internal),
	FUNCTION	7	mtree_float_same		(mtree_float, mtree_float),
	FUNCTION	8	mtree_float_distance	(internal, mtree_float, smallint, oid, internal);

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- _mtree_float
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

CREATE TYPE mtree_float_array;

CREATE OR REPLACE FUNCTION mtree_float_array_input(cstring)
RETURNS mtree_float_array
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_output(mtree_float_array)
RETURNS cstring
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE TYPE mtree_float_array (
	INPUT	= mtree_float_array_input,
	OUTPUT	= mtree_float_array_output,
	INTERNALLENGTH = VARIABLE,
	STORAGE	= extended
);

CREATE OR REPLACE FUNCTION mtree_float_array_consistent(internal, mtree_float_array, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_union(internal, internal)
RETURNS mtree_float_array
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_same(mtree_float_array, mtree_float_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_distance(internal, mtree_float_array, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_overlap_operator(mtree_float_array, mtree_float_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_contains_operator(mtree_float_array, mtree_float_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_contained_operator(mtree_float_array, mtree_float_array)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OR REPLACE FUNCTION mtree_float_array_distance_operator(mtree_float_array, mtree_float_array)
RETURNS float4
AS 'MODULE_PATHNAME'
LANGUAGE C STRICT IMMUTABLE;

CREATE OPERATOR = (
	LEFTARG		= mtree_float_array,
	RIGHTARG	= mtree_float_array,
	FUNCTION	= mtree_float_array_same,
	COMMUTATOR	= =
);

CREATE OPERATOR #&# (
	LEFTARG		= mtree_float_array,
	RIGHTARG	= mtree_float_array,
	FUNCTION	= mtree_float_array_overlap_operator,
	COMMUTATOR	= #&#
);

CREATE OPERATOR #># (
	LEFTARG		= mtree_float_array,
	RIGHTARG	= mtree_float_array,
	FUNCTION	= mtree_float_array_contains_operator,
	COMMUTATOR	= #<#
);

CREATE OPERATOR #<# (
	LEFTARG		= mtree_float_array,
	RIGHTARG	= mtree_float_array,
	FUNCTION	= mtree_float_array_contained_operator,
	COMMUTATOR	= #>#
);

CREATE OPERATOR <-> (
	LEFTARG		= mtree_float_array,
	RIGHTARG	= mtree_float_array,
	FUNCTION	= mtree_float_array_distance_operator,
	COMMUTATOR	= <->
);

CREATE OPERATOR CLASS gist_mtree_float_array_ops
DEFAULT FOR TYPE mtree_float_array USING gist AS
	OPERATOR	1	=	,
	OPERATOR	2	#&#	,
	OPERATOR	3	#>#	,
	OPERATOR	4	#<#	,
	OPERATOR	15	<->								(mtree_float_array, mtree_float_array) FOR ORDER BY float_ops,
	FUNCTION	1	mtree_float_array_consistent	(internal, mtree_float_array, smallint, oid, internal),
	FUNCTION	2	mtree_float_array_union			(internal, internal),
	FUNCTION	3	mtree_float_array_compress		(internal),
	FUNCTION	4	mtree_float_array_decompress	(internal),
	FUNCTION	5	mtree_float_array_penalty		(internal, internal, internal),
	FUNCTION	6	mtree_float_array_picksplit		(internal, internal),
	FUNCTION	7	mtree_float_array_same			(mtree_float_array, mtree_float_array),
	FUNCTION	8	mtree_float_array_distance		(internal, mtree_float_array, smallint, oid, internal);
