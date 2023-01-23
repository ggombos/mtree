/* contrib/mtree_gist/data/text_array.sql */

DROP TABLE IF EXISTS mtree_text_array_test CASCADE;

CREATE TABLE mtree_text_array_test (
	id	INTEGER,
	val	mtree_text_array
);

COPY mtree_text_array_test(id, val)
FROM '/home/postgres/test_files/text_array.csv'
DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS mtree_text_array_test_idx CASCADE;

CREATE INDEX mtree_text_array_test_idx ON mtree_text_array_test USING gist (
	val gist_mtree_text_array_ops (
		picksplit_strategy	= 'SamplingMinOverlapArea',
		union_strategy		= 'MinMaxDistance'
	)
);

SET enable_seqscan TO OFF;

EXPLAIN ANALYZE
SELECT id, val, (val <-> 'aaaaaaaaaa') AS dst
FROM mtree_text_array_test
ORDER BY (val <-> 'aaaaaaaaaa');

SELECT COUNT(*)
FROM mtree_text_array_test;

SELECT *
FROM mtree_text_array_test;

SELECT id, val, (val <-> 'aaaaaaaaaa') AS dst
FROM mtree_text_array_test
ORDER BY (val <-> 'aaaaaaaaaa')
LIMIT 10;

SELECT *
FROM mtree_text_array_test
WHERE val #<# 'aaaaaaaaaa';

SELECT id, val, (val <-> 'aaaaaaaaaa') AS dst
FROM mtree_text_array_test
ORDER BY (val <-> 'aaaaaaaaaa')
LIMIT 10;
