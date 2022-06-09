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

\copy fingerprints2 FROM '/home/ggombos/mtree/mtree_gist/data/fingerprint_data_10';

CREATE INDEX fingerprint2_gist_index ON fingerprints2 USING gist (hash_gist gist_mtree_text_ops) ;

SET enable_seqscan = off;

-- range query  fingerprint_data_10

-- SELECT * FROM fingerprints2 WHERE hash_gist #<# 'xxx1096972600000000000';

SELECT song_fk, hash, (hash_gist <-> 'xxx1096972600000000000') AS dst FROM fingerprints2 WHERE hash_gist <-> 'xxx1096972600000000000' <= 7;

-- SELECT song_fk, hash, offset2, (hash_gist <-> 'xxx1096972600000000000') AS dst FROM fingerprints2 ORDER BY (hash_gist <-> mtree_text('xxx1096972600000000000'));

 -- song_fk |        hash         | offset2 | dst
-------+---------------------+---------+-----
     -- 169 | 1096972600000000000 |      91 |   0
     -- 169 | 1591826000000000000 |     359 |   5
     -- 169 | 1057585200000000000 |     375 |   6
     -- 169 | 7893881000000000000 |    1194 |   7
     -- 169 | 5911270800000000000 |     578 |   7
     -- 169 | 1226851320000000000 |     614 |   7
     -- 169 | 2351023000000000000 |     767 |   8
     -- 169 | 7721238100000000000 |    1205 |   8
     -- 169 | 6557120000000000000 |     463 |   8
     -- 169 | 4721713000000000000 |    1171 |   8

