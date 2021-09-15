CREATE TABLE fingerprints2 (
  id  SERIAL PRIMARY KEY,
  song_fk INTEGER,
  hash	TEXT,
  offset2  INTEGER,
  hash_gist mtree_text
);

\copy fingerprints2 FROM '/home/postgres/fingerprintData';

CREATE INDEX fingerprint2_gist_index ON fingerprints2 USING gist (hash_gist mtree_text_opclass);

SET enable_seqscan = off;

SELECT song_fk, hash, offset2, (hash_gist <-> 'a3272361800000000000') AS dst FROM fingerprints2 ORDER BY dst USING LIMIT 30;
