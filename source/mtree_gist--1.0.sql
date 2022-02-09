/* contrib/mtree_gist/mtree_gist--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mtree_gist" to load this file. \quit

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- mtree_text (for text type)
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

-- Declaring data type
CREATE TYPE mtree_text;

-- Input function
CREATE OR REPLACE FUNCTION mtree_text_input(cstring)
RETURNS mtree_text
AS 'MODULE_PATHNAME', 'mtree_text_input'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Output function
CREATE OR REPLACE FUNCTION mtree_text_output(mtree_text)
RETURNS cstring
AS 'MODULE_PATHNAME', 'mtree_text_output'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Data type definition
CREATE TYPE mtree_text (
  INTERNALLENGTH = VARIABLE,
  INPUT = mtree_text_input,
  OUTPUT = mtree_text_output,
  STORAGE = extended
);

-- [Correctness] Consistent function
CREATE OR REPLACE FUNCTION mtree_text_consistent(internal, mtree_text, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_consistent'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Correctness] Union function
CREATE OR REPLACE FUNCTION mtree_text_union(internal, internal)
RETURNS mtree_text
AS 'MODULE_PATHNAME', 'mtree_text_union'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Correctness] Same function
CREATE OR REPLACE FUNCTION mtree_text_same(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_same'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Penalty function
CREATE OR REPLACE FUNCTION mtree_text_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_penalty'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Picksplit function
CREATE OR REPLACE FUNCTION mtree_text_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_picksplit'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Compress function
CREATE OR REPLACE FUNCTION mtree_text_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_compress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Decompress function
CREATE OR REPLACE FUNCTION mtree_text_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_decompress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Distance function
CREATE OR REPLACE FUNCTION mtree_text_distance_float(internal, mtree_text, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_text_distance_float'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Other] Distance function (Ordering operator)
CREATE OR REPLACE FUNCTION mtree_text_distance(mtree_text, mtree_text)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_text_distance'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Other] Overlap function
CREATE OR REPLACE FUNCTION mtree_text_overlap(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_overlap'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Other] Contains function
CREATE OR REPLACE FUNCTION mtree_text_contains(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_contains'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Other] Contained function
CREATE OR REPLACE FUNCTION mtree_text_contained(mtree_text, mtree_text)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_contained'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Other] Same function (internal)
CREATE OR REPLACE FUNCTION mtree_text_equals_first(mtree_text, mtree_text, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_equals_first'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Equals operator
CREATE OPERATOR = (
  COMMUTATOR = =,
  LEFTARG = mtree_text,
  RIGHTARG = mtree_text,
  PROCEDURE = mtree_text_same
);

-- Distance operator
CREATE OPERATOR <-> (
  COMMUTATOR = <->,
  LEFTARG = mtree_text,
  RIGHTARG = mtree_text,
  PROCEDURE = mtree_text_distance
);

-- Overlap operator
CREATE OPERATOR #&# (
  COMMUTATOR = #&#,
  LEFTARG = mtree_text,
  RIGHTARG = mtree_text,
  PROCEDURE = mtree_text_overlap
);

-- Contains operator
CREATE OPERATOR #># (
  COMMUTATOR = #<#,
  LEFTARG = mtree_text,
  RIGHTARG = mtree_text,
  PROCEDURE = mtree_text_contains
);

-- Contained operator
CREATE OPERATOR #<# (
  COMMUTATOR = #>#,
  LEFTARG = mtree_text,
  RIGHTARG = mtree_text,
  PROCEDURE = mtree_text_contained
);

-- Operator class
CREATE OPERATOR CLASS mtree_text_opclass
DEFAULT FOR TYPE mtree_text
USING GiST
AS
  OPERATOR 1 = ,
  OPERATOR 2 #&# ,
  OPERATOR 3 #># ,
  OPERATOR 4 #<# ,
  OPERATOR 15 <-> (mtree_text, mtree_text) FOR ORDER BY float_ops,
  FUNCTION 1 mtree_text_consistent (internal, mtree_text, smallint, oid, internal),
  FUNCTION 2 mtree_text_union (internal, internal),
  FUNCTION 3 mtree_text_compress (internal),
  FUNCTION 4 mtree_text_decompress (internal),
  FUNCTION 5 mtree_text_penalty (internal, internal, internal),
  FUNCTION 6 mtree_text_picksplit (internal, internal),
  FUNCTION 7 mtree_text_equals_first (mtree_text, mtree_text, internal),
  FUNCTION 8 mtree_text_distance_float (internal, mtree_text, smallint, oid, internal);

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- int8 (for int8)
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

-- Declaring data type
CREATE TYPE mtree_int8;

-- Input function
CREATE OR REPLACE FUNCTION mtree_int8_input(cstring)
RETURNS mtree_int8
AS 'MODULE_PATHNAME', 'mtree_int8_input'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Output function
CREATE OR REPLACE FUNCTION mtree_int8_output(mtree_int8)
RETURNS cstring
AS 'MODULE_PATHNAME', 'mtree_int8_output'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Data type definition
CREATE TYPE mtree_int8 (
  INTERNALLENGTH = VARIABLE,
  INPUT = mtree_int8_input,
  OUTPUT = mtree_int8_output,
  STORAGE = extended
);

-- [Correctness] Consistent function
CREATE OR REPLACE FUNCTION mtree_int8_consistent(internal, mtree_int8, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_consistent'
LANGUAGE C STRICT;

-- [Correctness] Union function
CREATE OR REPLACE FUNCTION mtree_int8_union(internal, internal)
RETURNS mtree_int8
AS 'MODULE_PATHNAME', 'mtree_int8_union'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Correctness] Same function
CREATE OR REPLACE FUNCTION mtree_int8_same(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_same'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Penalty function
CREATE OR REPLACE FUNCTION mtree_int8_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_penalty'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Picksplit function
CREATE OR REPLACE FUNCTION mtree_int8_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_picksplit'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Compress function
CREATE OR REPLACE FUNCTION mtree_int8_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_compress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Decompress function
CREATE OR REPLACE FUNCTION mtree_int8_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_decompress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Distance function
CREATE OR REPLACE FUNCTION mtree_int8_distance(internal, mtree_int8, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_int8_distance'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Overlap function
CREATE OR REPLACE FUNCTION mtree_int8_overlap_operator(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_overlap_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Contains function
CREATE OR REPLACE FUNCTION mtree_int8_contains_operator(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_contains_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Contained function
CREATE OR REPLACE FUNCTION mtree_int8_contained_operator(mtree_int8, mtree_int8)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_contained_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Distance function (Ordering operator)
CREATE OR REPLACE FUNCTION mtree_int8_distance_operator(mtree_int8, mtree_int8)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_int8_distance_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Equals operator
CREATE OPERATOR = (
  COMMUTATOR = =,
  LEFTARG = mtree_int8,
  RIGHTARG = mtree_int8,
  PROCEDURE = mtree_int8_same
);

-- Overlap operator
CREATE OPERATOR #&# (
  COMMUTATOR = #&#,
  LEFTARG = mtree_int8,
  RIGHTARG = mtree_int8,
  PROCEDURE = mtree_int8_overlap_operator
);

-- Contains operator
CREATE OPERATOR #># (
  COMMUTATOR = #<#,
  LEFTARG = mtree_int8,
  RIGHTARG = mtree_int8,
  PROCEDURE = mtree_int8_contains_operator
);

-- Contained operator
CREATE OPERATOR #<# (
  COMMUTATOR = #>#,
  LEFTARG = mtree_int8,
  RIGHTARG = mtree_int8,
  PROCEDURE = mtree_int8_contained_operator
);

-- Distance operator
CREATE OPERATOR <-> (
  COMMUTATOR = <->,
  LEFTARG = mtree_int8,
  RIGHTARG = mtree_int8,
  PROCEDURE = mtree_int8_distance_operator
);

-- Operator class
CREATE OPERATOR CLASS mtree_int8_opclass
DEFAULT FOR TYPE mtree_int8
USING GiST
AS
  -- Operators
  OPERATOR 1 = ,
  OPERATOR 2 #&# ,
  OPERATOR 3 #># ,
  OPERATOR 4 #<# ,
  OPERATOR 15 <-> (mtree_int8, mtree_int8) FOR ORDER BY float_ops,
  -- Functions
  FUNCTION 1 mtree_int8_consistent (internal, mtree_int8, smallint, oid, internal),
  FUNCTION 2 mtree_int8_union (internal, internal),
  FUNCTION 3 mtree_int8_compress (internal),
  FUNCTION 4 mtree_int8_decompress (internal),
  FUNCTION 5 mtree_int8_penalty (internal, internal, internal),
  FUNCTION 6 mtree_int8_picksplit (internal, internal),
  FUNCTION 7 mtree_int8_same (mtree_int8, mtree_int8),
  FUNCTION 8 mtree_int8_distance (internal, mtree_int8, smallint, oid, internal)
;

-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- int8 array (for int8 arrays)
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

-- Declaring data type
CREATE TYPE mtree_int8_array;

-- Input function
CREATE OR REPLACE FUNCTION mtree_int8_array_input(cstring)
RETURNS mtree_int8_array
AS 'MODULE_PATHNAME', 'mtree_int8_array_input'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Output function
CREATE OR REPLACE FUNCTION mtree_int8_array_output(mtree_int8_array)
RETURNS cstring
AS 'MODULE_PATHNAME', 'mtree_int8_array_output'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Data type definition
CREATE TYPE mtree_int8_array (
  INTERNALLENGTH = VARIABLE,
  INPUT = mtree_int8_array_input,
  OUTPUT = mtree_int8_array_output,
  STORAGE = extended
);

-- [Correctness] Consistent function
CREATE OR REPLACE FUNCTION mtree_int8_array_consistent(internal, mtree_int8_array, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_array_consistent'
LANGUAGE C STRICT;

-- [Correctness] Union function
CREATE OR REPLACE FUNCTION mtree_int8_array_union(internal, internal)
RETURNS mtree_int8_array
AS 'MODULE_PATHNAME', 'mtree_int8_array_union'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Correctness] Same function
CREATE OR REPLACE FUNCTION mtree_int8_array_same(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_array_same'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Penalty function
CREATE OR REPLACE FUNCTION mtree_int8_array_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_array_penalty'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Picksplit function
CREATE OR REPLACE FUNCTION mtree_int8_array_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_array_picksplit'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Compress function
CREATE OR REPLACE FUNCTION mtree_int8_array_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_array_compress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Decompress function
CREATE OR REPLACE FUNCTION mtree_int8_array_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_int8_array_decompress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Distance function
CREATE OR REPLACE FUNCTION mtree_int8_array_distance(internal, mtree_int8_array, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_int8_array_distance'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Overlap function
CREATE OR REPLACE FUNCTION mtree_int8_array_overlap_operator(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_array_overlap_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Contains function
CREATE OR REPLACE FUNCTION mtree_int8_array_contains_operator(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_array_contains_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Contained function
CREATE OR REPLACE FUNCTION mtree_int8_array_contained_operator(mtree_int8_array, mtree_int8_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_int8_array_contained_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Distance function (Ordering operator)
CREATE OR REPLACE FUNCTION mtree_int8_array_distance_operator(mtree_int8_array, mtree_int8_array)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_int8_array_distance_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Equals operator
CREATE OPERATOR = (
  COMMUTATOR = =,
  LEFTARG = mtree_int8_array,
  RIGHTARG = mtree_int8_array,
  PROCEDURE = mtree_int8_array_same
);

-- Overlap operator
CREATE OPERATOR #&# (
  COMMUTATOR = #&#,
  LEFTARG = mtree_int8_array,
  RIGHTARG = mtree_int8_array,
  PROCEDURE = mtree_int8_array_overlap_operator
);

-- Contains operator
CREATE OPERATOR #># (
  COMMUTATOR = #<#,
  LEFTARG = mtree_int8_array,
  RIGHTARG = mtree_int8_array,
  PROCEDURE = mtree_int8_array_contains_operator
);

-- Contained operator
CREATE OPERATOR #<# (
  COMMUTATOR = #>#,
  LEFTARG = mtree_int8_array,
  RIGHTARG = mtree_int8_array,
  PROCEDURE = mtree_int8_array_contained_operator
);

-- Distance operator
CREATE OPERATOR <-> (
  COMMUTATOR = <->,
  LEFTARG = mtree_int8_array,
  RIGHTARG = mtree_int8_array,
  PROCEDURE = mtree_int8_array_distance_operator
);

-- Operator class
CREATE OPERATOR CLASS mtree_int8_array_opclass
DEFAULT FOR TYPE mtree_int8_array
USING GiST
AS
  -- Operators
  OPERATOR 1 = ,
  OPERATOR 2 #&# ,
  OPERATOR 3 #># ,
  OPERATOR 4 #<# ,
  OPERATOR 15 <-> (mtree_int8_array, mtree_int8_array) FOR ORDER BY float_ops,
  -- Functions
  FUNCTION 1 mtree_int8_array_consistent (internal, mtree_int8_array, smallint, oid, internal),
  FUNCTION 2 mtree_int8_array_union (internal, internal),
  FUNCTION 3 mtree_int8_array_compress (internal),
  FUNCTION 4 mtree_int8_array_decompress (internal),
  FUNCTION 5 mtree_int8_array_penalty (internal, internal, internal),
  FUNCTION 6 mtree_int8_array_picksplit (internal, internal),
  FUNCTION 7 mtree_int8_array_same (mtree_int8_array, mtree_int8_array),
  FUNCTION 8 mtree_int8_array_distance (internal, mtree_int8_array, smallint, oid, internal)
;


-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- int8 array (for int8 arrays)
-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --

-- Declaring data type
CREATE TYPE mtree_text_array;

-- Input function
CREATE OR REPLACE FUNCTION mtree_text_array_input(cstring)
RETURNS mtree_text_array
AS 'MODULE_PATHNAME', 'mtree_text_array_input'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Output function
CREATE OR REPLACE FUNCTION mtree_text_array_output(mtree_text_array)
RETURNS cstring
AS 'MODULE_PATHNAME', 'mtree_text_array_output'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Data type definition
CREATE TYPE mtree_text_array (
  INTERNALLENGTH = VARIABLE,
  INPUT = mtree_text_array_input,
  OUTPUT = mtree_text_array_output,
  STORAGE = extended
);

-- [Correctness] Consistent function
CREATE OR REPLACE FUNCTION mtree_text_array_consistent(internal, mtree_text_array, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_array_consistent'
LANGUAGE C STRICT;

-- [Correctness] Union function
CREATE OR REPLACE FUNCTION mtree_text_array_union(internal, internal)
RETURNS mtree_text_array
AS 'MODULE_PATHNAME', 'mtree_text_array_union'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Correctness] Same function
CREATE OR REPLACE FUNCTION mtree_text_array_same(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_array_same'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Penalty function
CREATE OR REPLACE FUNCTION mtree_text_array_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_array_penalty'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Efficiency] Picksplit function
CREATE OR REPLACE FUNCTION mtree_text_array_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_array_picksplit'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Compress function
CREATE OR REPLACE FUNCTION mtree_text_array_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_array_compress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Decompress function
CREATE OR REPLACE FUNCTION mtree_text_array_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'mtree_text_array_decompress'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Optional] Distance function
CREATE OR REPLACE FUNCTION mtree_text_array_distance(internal, mtree_text_array, smallint, oid, internal)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_text_array_distance'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Overlap function
CREATE OR REPLACE FUNCTION mtree_text_array_overlap_operator(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_array_overlap_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Contains function
CREATE OR REPLACE FUNCTION mtree_text_array_contains_operator(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_array_contains_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Contained function
CREATE OR REPLACE FUNCTION mtree_text_array_contained_operator(mtree_text_array, mtree_text_array)
RETURNS bool
AS 'MODULE_PATHNAME', 'mtree_text_array_contained_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Operator] Distance function (Ordering operator)
CREATE OR REPLACE FUNCTION mtree_text_array_distance_operator(mtree_text_array, mtree_text_array)
RETURNS float4
AS 'MODULE_PATHNAME', 'mtree_text_array_distance_operator'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- Equals operator
CREATE OPERATOR = (
  COMMUTATOR = =,
  LEFTARG = mtree_text_array,
  RIGHTARG = mtree_text_array,
  PROCEDURE = mtree_text_array_same
);

-- Overlap operator
CREATE OPERATOR #&# (
  COMMUTATOR = #&#,
  LEFTARG = mtree_text_array,
  RIGHTARG = mtree_text_array,
  PROCEDURE = mtree_text_array_overlap_operator
);

-- Contains operator
CREATE OPERATOR #># (
  COMMUTATOR = #<#,
  LEFTARG = mtree_text_array,
  RIGHTARG = mtree_text_array,
  PROCEDURE = mtree_text_array_contains_operator
);

-- Contained operator
CREATE OPERATOR #<# (
  COMMUTATOR = #>#,
  LEFTARG = mtree_text_array,
  RIGHTARG = mtree_text_array,
  PROCEDURE = mtree_text_array_contained_operator
);

-- Distance operator
CREATE OPERATOR <-> (
  COMMUTATOR = <->,
  LEFTARG = mtree_text_array,
  RIGHTARG = mtree_text_array,
  PROCEDURE = mtree_text_array_distance_operator
);

-- Operator class
CREATE OPERATOR CLASS mtree_text_array_opclass
DEFAULT FOR TYPE mtree_text_array
USING GiST
AS
  -- Operators
  OPERATOR 1 = ,
  OPERATOR 2 #&# ,
  OPERATOR 3 #># ,
  OPERATOR 4 #<# ,
  OPERATOR 15 <-> (mtree_text_array, mtree_text_array) FOR ORDER BY float_ops,
  -- Functions
  FUNCTION 1 mtree_text_array_consistent (internal, mtree_text_array, smallint, oid, internal),
  FUNCTION 2 mtree_text_array_union (internal, internal),
  FUNCTION 3 mtree_text_array_compress (internal),
  FUNCTION 4 mtree_text_array_decompress (internal),
  FUNCTION 5 mtree_text_array_penalty (internal, internal, internal),
  FUNCTION 6 mtree_text_array_picksplit (internal, internal),
  FUNCTION 7 mtree_text_array_same (mtree_text_array, mtree_text_array),
  FUNCTION 8 mtree_text_array_distance (internal, mtree_text_array, smallint, oid, internal)
;

