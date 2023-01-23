/* contrib/mtree_gist/data/int8.sql */

DROP TABLE IF EXISTS INT8_TEST CASCADE;
CREATE TABLE INT8_TEST (
  id  INTEGER,
  val mtree_int8
);
COPY INT8_TEST(id, val) FROM '/home/postgres/test_files/int8.csv' DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS INT8_TEST_IDX CASCADE;
CREATE INDEX INT8_TEST_IDX ON INT8_TEST USING GiST (val gist_mtree_int8_ops);

SET enable_seqscan TO OFF;

SELECT COUNT(*) FROM INT8_TEST;

SELECT * FROM INT8_TEST;

SELECT id, val, (val <-> '0') AS dst FROM INT8_TEST ORDER BY (val <-> '0');

SELECT * FROM INT8_TEST WHERE val #<# '0';
