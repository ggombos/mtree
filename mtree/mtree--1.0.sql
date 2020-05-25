--DROP TYPE mus_gist CASCADE;
-- CREATE TYPE mus_gist;

CREATE OR REPLACE FUNCTION mus_gist_in(cstring) RETURNS mus_gist AS
  'MODULE_PATHNAME', 'mus_gist_in'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_out(mus_gist) RETURNS cstring AS
  'MODULE_PATHNAME', 'mus_gist_out'
  LANGUAGE c IMMUTABLE STRICT;

CREATE TYPE mus_gist (
   internallength = VARIABLE,
   input = mus_gist_in,
   output = mus_gist_out,
   -- receive = mus_gist_recv,
   -- send = mus_gist_send,
   alignment = int4
);

CREATE OR REPLACE FUNCTION mus_gist_same(mus_gist, mus_gist) RETURNS bool AS
  'MODULE_PATHNAME', 'mus_gist_same'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_distance(mus_gist, mus_gist) RETURNS integer AS
  'MODULE_PATHNAME', 'mus_gist_distance'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_overlap(mus_gist, mus_gist) RETURNS bool AS
  'MODULE_PATHNAME', 'mus_gist_overlap'
  LANGUAGE c IMMUTABLE STRICT;
  
CREATE OR REPLACE FUNCTION mus_gist_contains(mus_gist, mus_gist) RETURNS bool AS
  'MODULE_PATHNAME', 'mus_gist_contains'
  LANGUAGE c IMMUTABLE STRICT;
  
CREATE OR REPLACE FUNCTION mus_gist_contained(mus_gist, mus_gist) RETURNS bool AS
  'MODULE_PATHNAME', 'mus_gist_contained'
  LANGUAGE c IMMUTABLE STRICT;

-- DROP OPERATOR = (mus_gist, mus_gist);
CREATE OPERATOR = (
   leftarg = mus_gist,
   rightarg = mus_gist,
   procedure = mus_gist_same,
   commutator = =
);

-- DROP OPERATOR <-> (mus_gist, mus_gist);
CREATE OPERATOR <-> (
   leftarg = mus_gist,
   rightarg = mus_gist,
   procedure = mus_gist_distance,
   commutator = <->
);

-- DROP OPERATOR #&# (mus_gist, mus_gist);
CREATE OPERATOR #&# (
   leftarg = mus_gist,
   rightarg = mus_gist,
   procedure = mus_gist_overlap,
   commutator = #&#
);

-- DROP OPERATOR #># (mus_gist, mus_gist);
CREATE OPERATOR #># (
   leftarg = mus_gist,
   rightarg = mus_gist,
   procedure = mus_gist_contains,
   commutator = #<#
   -- operator #># is the commutator of operator #<# because A #># B equals to B #<# A for all A, B
);

-- DROP OPERATOR #<# (mus_gist, mus_gist);
CREATE OPERATOR #<# (
   leftarg = mus_gist,
   rightarg = mus_gist,
   procedure = mus_gist_contained,
   commutator = #>#
   -- operator #<# is the commutator of operator #># because A #<# B equals to B #># A for all A, B
);

-- CREATE SCHEMA mus;
-- GRANT ALL ON SCHEMA mus TO public;

CREATE OR REPLACE FUNCTION mus_gist_compress(internal) RETURNS internal AS
  'MODULE_PATHNAME', 'mus_gist_compress'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_decompress(internal) RETURNS internal AS
  'MODULE_PATHNAME', 'mus_gist_decompress'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_consistent(internal, mus_gist, smallint, oid, internal) RETURNS bool AS
  'MODULE_PATHNAME', 'mus_gist_consistent'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_union(internal, internal) RETURNS mus_gist AS
  'MODULE_PATHNAME', 'gist_union'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_penalty(internal, internal, internal) RETURNS internal AS
  'MODULE_PATHNAME', 'mus_gist_penalty'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_picksplit(internal, internal) RETURNS internal AS
  'MODULE_PATHNAME', 'gist_picksplit'
  LANGUAGE c IMMUTABLE STRICT;
  
CREATE OR REPLACE FUNCTION mus_gist_same_internal(mus_gist, mus_gist, internal) RETURNS internal AS
 'MODULE_PATHNAME', 'mus_gist_same_internal'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION mus_gist_distance_internal(internal, mus_gist, smallint, oid, internal) RETURNS float8 AS
 'MODULE_PATHNAME', 'mus_gist_distance_internal'
  LANGUAGE c IMMUTABLE STRICT;

CREATE OPERATOR CLASS mus_gist_opclass
  DEFAULT FOR TYPE mus_gist USING gist AS
    OPERATOR 1 = ,
    OPERATOR 2 #&# ,
    OPERATOR 3 #># ,
    OPERATOR 4 #<# ,
    OPERATOR 5 <-> (mus_gist, mus_gist) FOR ORDER BY integer_ops,
    FUNCTION 1 mus_gist_consistent (internal, mus_gist, smallint, oid, internal),
    FUNCTION 2 mus_gist_union (internal, internal),
    FUNCTION 3 mus_gist_compress (internal),
    FUNCTION 4 mus_gist_decompress (internal),
    FUNCTION 5 mus_gist_penalty (internal, internal, internal),
    FUNCTION 6 mus_gist_picksplit (internal, internal),
    FUNCTION 7 mus_gist_same_internal (mus_gist, mus_gist, internal),
    FUNCTION 8 mus_gist_distance_internal (internal, mus_gist, smallint, oid, internal);

-- Creating table and importing data

-- DROP TABLE songs;
-- CREATE TABLE songs (x integer, data mus_gist);
 -- COPY songs (x, data) FROM '/home/ggombos/mtreeproject/inputData/merged_small.csv' DELIMITER ',' CSV HEADER;
-- COPY songs (x, data) FROM '/home/ggombos/mtreeproject/inputData/merged.csv' DELIMITER ',' CSV HEADER;
-- 
-- 
-- --- TEXT ---
-- 
-- CREATE OR REPLACE FUNCTION text_distance(text, text) RETURNS integer AS
--   'MODULE_PATHNAME', 'text_distance'
--   LANGUAGE c IMMUTABLE STRICT;
-- 
-- DROP OPERATOR <-> (text, text);
-- CREATE OPERATOR <-> (
--    leftarg = text,
--    rightarg = text,
--    procedure = text_distance,
--    commutator = <->
-- );

-- DROP TABLE songs_text;
-- CREATE TABLE songs_text (x integer, data text);
-- COPY songs_text (x, data) FROM '/home/ggombos/mtreeproject/inputData/merged_small.csv' DELIMITER ',' CSV HEADER;
-- COPY songs_text (x, data) FROM '/home/ggombos/mtreeproject/inputData/merged.csv' DELIMITER ',' CSV HEADER;

-- vacuum analyze mus_gen_sequence;

-- CREATE UNIQUE INDEX gen_sequence_idx ON mus_gen_sequence (id);
-- CREATE INDEX gen_sequence_ndtree_idx ON mus_gen_sequence USING GIST (seq gist_ndtree_ops) WITH (FILLFACTOR = 100, BUFFERING = OFF);
-- CREATE INDEX index_test ON songs USING GIST (data mus_gist_opclass);
-- CREATE INDEX index_test ON songs USING gist (data mus_gist_opclass);
-- CREATE INDEX btree_test ON songs_text USING btree (data);
