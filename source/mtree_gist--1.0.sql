/* contrib/mtree_gist/mtree_gist--1.0.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mtree_gist" to load this file. \quit

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

-- [Optional] Distance function (internal)
CREATE OR REPLACE FUNCTION mtree_text_distance_float(internal, mtree_text, smallint, oid, internal)
RETURNS float8
AS 'MODULE_PATHNAME', 'mtree_text_distance_float'
LANGUAGE C STRICT IMMUTABLE PARALLEL SAFE;

-- [Other] Distance function
CREATE OR REPLACE FUNCTION mtree_text_distance(mtree_text, mtree_text)
RETURNS float8
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
USING gist
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
