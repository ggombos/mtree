/* contrib/mtree_gist/data/text_array.sql */

DROP TABLE IF EXISTS TEXT_ARRAY_TEST CASCADE;
CREATE TABLE TEXT_ARRAY_TEST (
  id  INTEGER,
  val mtree_text_array
);
COPY TEXT_ARRAY_TEST(id, val) FROM '/home/postgres/test_files/text_array.csv' DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS TEXT_ARRAY_TEST_IDX CASCADE;
CREATE INDEX TEXT_ARRAY_TEST_IDX ON TEXT_ARRAY_TEST USING GiST (val mtree_text_array_opclass);

SET enable_seqscan TO OFF;

SELECT * FROM TEXT_ARRAY_TEST;

SELECT id, val, (val <-> '0') AS dst FROM TEXT_ARRAY_TEST ORDER BY (val <-> '0');

SELECT * FROM TEXT_ARRAY_TEST WHERE val #<# '0';
