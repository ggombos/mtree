/* contrib/mtree_gist/data/float4.sql */

DROP TABLE IF EXISTS FLOAT4_TEST CASCADE;
CREATE TABLE FLOAT4_TEST (
  id  INTEGER,
  val mtree_float
);
COPY FLOAT4_TEST(id, val) FROM '/home/postgres/test_files/float4.csv' DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS FLOAT4_TEST_IDX CASCADE;
CREATE INDEX FLOAT4_TEST_IDX ON FLOAT4_TEST USING GiST (val gist_mtree_float_ops);

SET enable_seqscan TO OFF;

SELECT COUNT(*) FROM FLOAT4_TEST;

SELECT * FROM FLOAT4_TEST;

SELECT id, val, (val <-> '0') AS dst FROM FLOAT4_TEST ORDER BY (val <-> '0');

SELECT * FROM FLOAT4_TEST WHERE val #<# '0';
