SET enable_seqscan TO OFF;

EXPLAIN ANALYZE
SELECT id, val, (val <-> 'ghwfxakyka') AS dst
FROM mtree_text_test
ORDER BY (val <-> 'ghwfxakyka')
LIMIT 10;
