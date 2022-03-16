/* contrib/mtree_gist/data/text.sql */

SET client_min_messages TO WARNING;

DROP TABLE IF EXISTS TEXT_TEST CASCADE;
CREATE TABLE TEXT_TEST (
  id  INTEGER,
  val mtree_text
);
COPY TEXT_TEST(id, val) FROM '/home/postgres/test_files/text.csv' DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS TEXT_TEST_IDX CASCADE;
CREATE INDEX TEXT_TEST_IDX ON TEXT_TEST USING GiST (val mtree_text_opclass);

SET enable_seqscan TO OFF;

SELECT COUNT(*) FROM TEXT_TEST;

SELECT * FROM TEXT_TEST;

SELECT id, val, (val <-> '0') AS dst FROM TEXT_TEST ORDER BY (val <-> '0');

SELECT * FROM TEXT_TEST WHERE val #<# '0';
