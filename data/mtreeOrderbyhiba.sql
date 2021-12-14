DROP TABLE IF EXISTS fingerprint_order_by_test;
CREATE TABLE fingerprint_order_by_test (
  id        SERIAL PRIMARY KEY,
  song_fk   INTEGER,
  hash	    TEXT,
  offset2   INTEGER,
  hash_gist mtree_text
);

\copy fingerprint_order_by_test FROM '/home/postgres/fingerprint_data_10000';

CREATE INDEX fingerprint_order_by_test_index ON fingerprint_order_by_test USING GiST (hash_gist mtree_text_opclass);

SET enable_seqscan = OFF;

-- EXPLAIN ANALYZE
SELECT song_fk, hash, offset2, (hash_gist <-> 'xxx3272361800000000000') AS dst FROM fingerprint_order_by_test ORDER BY (hash_gist <-> 'xxx3272361800000000000');
