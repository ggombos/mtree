/* contrib/mtree_gist/mtree_gist--1.0.sql */

-- Complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION mtree_gist" to load this file. \quit

-- type, input and output functions
CREATE TYPE gmtreekey_var;

CREATE FUNCTION gmtreekey_var_in(cstring)
RETURNS gmtreekey_var
AS 'MODULE_PATHNAME', 'gmtreekey_in'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gmtreekey_var_out(gmtreekey_var)
RETURNS cstring
AS 'MODULE_PATHNAME', 'gmtreekey_out'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE gmtreekey_var (
  INTERNALLENGTH = VARIABLE,
  INPUT  = gmtreekey_var_in,
  OUTPUT = gmtreekey_var_out,
	STORAGE = EXTENDED
);

-- text operations
CREATE FUNCTION gmt_text_consistent(internal,text,int2,oid,internal)
RETURNS bool
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gmt_text_compress(internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gmt_text_penalty(internal,internal,internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gmt_text_picksplit(internal, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gmt_text_union(internal, internal)
RETURNS gmtreekey_var
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION gmt_text_same(gmtreekey_var, gmtreekey_var, internal)
RETURNS internal
AS 'MODULE_PATHNAME'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_text_distance(gmtreekey_var, gmtreekey_var) -- TODO
RETURNS integer
AS 'MODULE_PATHNAME', 'gmt_text_distance'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_text_overlap(gmtreekey_var, gmtreekey_var) -- TODO
RETURNS bool
AS 'MODULE_PATHNAME', 'gmt_text_overlap'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_text_contains(gmtreekey_var, gmtreekey_var) -- TODO
RETURNS bool
AS 'MODULE_PATHNAME', 'gmt_text_contains'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_text_contained(gmtreekey_var, gmtreekey_var) -- TODO
RETURNS bool
AS 'MODULE_PATHNAME', 'gmt_text_contained'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_text_distance_internal(internal, gmtreekey_var, smallint, oid, internal) -- TODO
RETURNS float8
AS 'MODULE_PATHNAME', 'gmt_text_distance_internal'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_var_decompress(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'gmt_var_decompress'
LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION gmt_var_fetch(internal)
RETURNS internal
AS 'MODULE_PATHNAME', 'gmt_var_fetch'
LANGUAGE C IMMUTABLE STRICT;

-- text operators
CREATE OPERATOR = (
  COMMUTATOR = =,
  LEFTARG = text,
  RIGHTARG = text,
  PROCEDURE = gmt_text_same
);

CREATE OPERATOR <-> (
  COMMUTATOR = <->,
  LEFTARG = text,
  RIGHTARG = text,
  PROCEDURE = gmt_text_distance
);

CREATE OPERATOR #&# (
  COMMUTATOR = #&#,
  LEFTARG = text,
  RIGHTARG = text,
  PROCEDURE = gmt_text_overlap
);

CREATE OPERATOR #># (
  COMMUTATOR = #<#,
  LEFTARG = text,
  RIGHTARG = text,
  PROCEDURE = gmt_text_contains
);

CREATE OPERATOR #<# (
  COMMUTATOR = #>#,
  LEFTARG = text,
  RIGHTARG = text,
  PROCEDURE = gmt_text_contained
);

CREATE OPERATOR CLASS gist_text_ops
DEFAULT FOR TYPE text USING gist
AS
  OPERATOR  1  <   ,
  OPERATOR  2  <=  ,
  OPERATOR  3  >=  ,
  OPERATOR  4  >   ,
  OPERATOR  5  =   ,
  OPERATOR  6  #&# ,
  OPERATOR  7  #># ,
  OPERATOR  8  #<# ,
  OPERATOR  9  <-> (gmtreekey_var, gmtreekey_var) FOR ORDER BY integer_ops,
  FUNCTION  1  gmt_text_consistent (internal, text, int2, oid, internal),
  FUNCTION  2  gmt_text_union (internal, internal),
  FUNCTION  3  gmt_text_compress (internal),
  FUNCTION  4  gmt_var_decompress (internal),
  FUNCTION  5  gmt_text_penalty (internal, internal, internal),
  FUNCTION  6  gmt_text_picksplit (internal, internal),
  FUNCTION  7  gmt_text_same (gmtreekey_var, gmtreekey_var, internal),
  STORAGE      gmtreekey_var;

ALTER OPERATOR FAMILY gist_text_ops USING gist ADD
  OPERATOR  6  <> (text, text) ,
  FUNCTION  9 (text, text) gmt_var_fetch (internal) ;
