/* contrib/mtree_gist/data/text.sql */

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

SELECT id, val, (val <-> 'aaaaaaaaaa') AS dst FROM TEXT_TEST ORDER BY (val <-> 'aaaaaaaaaa'), id;

SELECT * FROM TEXT_TEST WHERE val #<# 'aaaaaaaaaa';
