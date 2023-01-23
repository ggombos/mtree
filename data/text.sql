/* contrib/mtree_gist/data/text.sql */

DROP TABLE IF EXISTS mtree_text_test CASCADE;

CREATE TABLE mtree_text_test (
	id	INTEGER,
	val	mtree_text
);

COPY mtree_text_test(id, val)
FROM '/home/postgres/test_files/text.csv'
DELIMITER ';' CSV HEADER;

DROP INDEX IF EXISTS mtree_text_test_idx CASCADE;

CREATE INDEX mtree_text_test_idx ON mtree_text_test USING gist (
	val gist_mtree_text_ops (
		picksplit_strategy	= 'SamplingMinOverlapArea',
		union_strategy		= 'MinMaxDistance'
	)
);

SET enable_seqscan TO OFF;

SELECT COUNT(*)
FROM mtree_text_test;

SELECT id, val, (val <-> 'ghwfxakyka') AS dst
FROM mtree_text_test
ORDER BY (val <-> 'ghwfxakyka')
LIMIT 10;

SELECT *
FROM mtree_text_test
WHERE val #<# 'ghwfxakyka';
