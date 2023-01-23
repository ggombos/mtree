/* contrib/mtree_gist/data/int8_array.sql */

DROP TABLE IF EXISTS INT8_ARRAY_TEST CASCADE;
CREATE TABLE INT8_ARRAY_TEST (
  id  INTEGER,
  val mtree_int8_array
);
COPY INT8_ARRAY_TEST(id, val) FROM '/home/postgres/test_files/int8_array.csv' DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS INT8_ARRAY_TEST_IDX CASCADE;
CREATE INDEX INT8_ARRAY_TEST_IDX ON INT8_ARRAY_TEST USING GiST (val gist_mtree_int8_array_ops);

SET enable_seqscan TO OFF;

SELECT COUNT(*) FROM INT8_ARRAY_TEST;

SELECT * FROM INT8_ARRAY_TEST;

SELECT id, val, (val <-> '0') AS dst FROM INT8_ARRAY_TEST ORDER BY (val <-> '0');
