DROP INDEX fingerprint2_gist_index;
DROP TABLE fingerprints2;
DROP EXTENSION mtree_gist;
SELECT * FROM pg_extension;

CREATE EXTENSION mtree_gist;
SELECT * FROM pg_extension;

CREATE TABLE fingerprints2 (
  id  SERIAL PRIMARY KEY,
  song_fk INTEGER,
  hash	TEXT,
  offset2  INTEGER,
  hash_gist mtree_text
);

\copy fingerprints2 FROM '/home/ggombos/mtree/mtree_gist/data/fingerprint_data_10000';
-- \copy fingerprints2 FROM '/home/ggombos/mtree/mtree_gist/data/fingerprint_data_1002156';

CREATE INDEX fingerprint2_gist_index ON fingerprints2 USING gist (hash_gist mtree_text_opclass) ;

SET enable_seqscan = off;


-- EXPLAIN ANALYZE 
SELECT song_fk, hash, offset2, (hash_gist <-> 'xxx3272361800000000000') AS dst FROM fingerprints2 ORDER BY (hash_gist <-> 'xxx3272361800000000000');

-- SET enable_seqscan = off;
-- EXPLAIN ANALYZE 
-- SELECT song_fk, hash, offset2, (hash_gist <-> mtree_text('xxx3272361800000000000')) AS dst FROM fingerprints2 ORDER BY (hash_gist <-> 'xxx3272361800000000000');

-- SET enable_seqscan = on;
-- EXPLAIN ANALYZE SELECT song_fk, hash, offset2, (hash_gist <-> mtree_text('xxx3272361800000000000')) AS dst FROM fingerprints2 ORDER BY (hash_gist <-> mtree_text('xxx3272361800000000000'));
