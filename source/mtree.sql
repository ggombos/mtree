-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mtree%index%" to load this file. \quit

-- Declaring data type
CREATE TYPE music_gist;

-- Input function
CREATE OR REPLACE FUNCTION music_gist_input(cstring)
RETURNS music_gist
AS 'MODULE_PATHNAME', 'music_gist_input'
LANGUAGE C IMMUTABLE STRICT;

-- Output function
CREATE OR REPLACE FUNCTION music_gist_output(music_gist)
RETURNS cstring
AS 'MODULE_PATHNAME', 'music_gist_output'
LANGUAGE C IMMUTABLE STRICT;

-- Data type definition
CREATE TYPE music_gist (
  INTERNALLENGTH = VARIABLE,
  INPUT = music_gist_input,
  OUTPUT = music_gist_output
);

-- [Correctness] Consistent function
CREATE OR REPLACE FUNCTION music_gist_consistent(internal, music_gist, smallint, oid, internal)
RETURNS bool
AS 'MODULE_PATHNAME', 'music_gist_consistent'
LANGUAGE C IMMUTABLE STRICT;

-- [Correctness] Union function
CREATE OR REPLACE FUNCTION music_gist_union(internal, internal)
RETURNS music_gist
AS 'MODULE_PATHNAME', 'gist_union'
LANGUAGE C IMMUTABLE STRICT;

-- [Correctness] Same function
CREATE OR REPLACE FUNCTION music_gist_same(music_gist, music_gist)
RETURNS bool
AS 'MODULE_PATHNAME', 'music_gist_same'
LANGUAGE C IMMUTABLE STRICT;

-- [Efficiency] Penalty function
CREATE OR REPLACE FUNCTION music_gist_penalty(internal, internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'music_gist_penalty'
LANGUAGE C IMMUTABLE STRICT;

-- [Efficiency] Picksplit function
CREATE OR REPLACE FUNCTION music_gist_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'gist_picksplit'
LANGUAGE C IMMUTABLE STRICT;

-- [Optional] Compress function
CREATE OR REPLACE FUNCTION music_gist_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'music_gist_compress'
LANGUAGE C IMMUTABLE STRICT;

-- [Optional] Decompress function
CREATE OR REPLACE FUNCTION music_gist_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'music_gist_decompress'
LANGUAGE C IMMUTABLE STRICT;

-- [Optional] Distance function (internal)
CREATE OR REPLACE FUNCTION music_gist_distance_internal(internal, music_gist, smallint, oid, internal)
RETURNS float8
AS 'MODULE_PATHNAME', 'music_gist_distance_internal'
LANGUAGE C IMMUTABLE STRICT;

-- [Other] Distance function
CREATE OR REPLACE FUNCTION music_gist_distance(music_gist, music_gist)
RETURNS integer
AS 'MODULE_PATHNAME', 'music_gist_distance'
LANGUAGE C IMMUTABLE STRICT;

-- [Other] Overlap function
CREATE OR REPLACE FUNCTION music_gist_overlap(music_gist, music_gist)
RETURNS bool
AS 'MODULE_PATHNAME', 'music_gist_overlap'
LANGUAGE C IMMUTABLE STRICT;

-- [Other] Contains function
CREATE OR REPLACE FUNCTION music_gist_contains(music_gist, music_gist)
RETURNS bool
AS 'MODULE_PATHNAME', 'music_gist_contains'
LANGUAGE C IMMUTABLE STRICT;

-- [Other] Contained function
CREATE OR REPLACE FUNCTION music_gist_contained(music_gist, music_gist)
RETURNS bool
AS 'MODULE_PATHNAME', 'music_gist_contained'
LANGUAGE C IMMUTABLE STRICT;

-- [Other] Same function (internal)
CREATE OR REPLACE FUNCTION music_gist_same_internal(music_gist, music_gist, internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'music_gist_same_internal'
LANGUAGE C IMMUTABLE STRICT;

-- Equals operator
CREATE OPERATOR = (
  COMMUTATOR = =,
  LEFTARG = music_gist,
  RIGHTARG = music_gist,
  PROCEDURE = music_gist_same
);

-- Distance operator
CREATE OPERATOR <-> (
  COMMUTATOR = <->,
  LEFTARG = music_gist,
  RIGHTARG = music_gist,
  PROCEDURE = music_gist_distance
);

-- Overlap operator
CREATE OPERATOR #&# (
  COMMUTATOR = #&#,
  LEFTARG = music_gist,
  RIGHTARG = music_gist,
  PROCEDURE = music_gist_overlap
);

-- Contains operator
CREATE OPERATOR #># (
  COMMUTATOR = #<#,
  LEFTARG = music_gist,
  RIGHTARG = music_gist,
  PROCEDURE = music_gist_contains
);

-- Contained operator
CREATE OPERATOR #<# (
  COMMUTATOR = #>#,
  LEFTARG = music_gist,
  RIGHTARG = music_gist,
  PROCEDURE = music_gist_contained
);

-- Operator class
CREATE OPERATOR CLASS music_gist_opclass
DEFAULT FOR TYPE music_gist
USING gist
AS
  OPERATOR 1 = ,
  OPERATOR 2 #&# ,
  OPERATOR 3 #># ,
  OPERATOR 4 #<# ,
  OPERATOR 5 <-> (music_gist, music_gist) FOR ORDER BY integer_ops,
  FUNCTION 1 music_gist_consistent (internal, music_gist, smallint, oid, internal),
  FUNCTION 2 music_gist_union (internal, internal),
  FUNCTION 3 music_gist_compress (internal),
  FUNCTION 4 music_gist_decompress (internal),
  FUNCTION 5 music_gist_penalty (internal, internal, internal),
  FUNCTION 6 music_gist_picksplit (internal, internal),
  FUNCTION 7 music_gist_same_internal (music_gist, music_gist, internal),
  FUNCTION 8 music_gist_distance_internal (internal, music_gist, smallint, oid, internal);
