SET enable_seqscan = OFF;

EXPLAIN ANALYZE SELECT song_fk, hash, offset2, (hash_gist <-> 'a3272361800000000000') AS dst FROM Fingerprint_1000 ORDER BY dst;

EXPLAIN ANALYZE SELECT song_fk, hash, offset2, (hash_gist <-> 'a3272361800000000000') AS dst FROM Fingerprint_10000 ORDER BY dst;

EXPLAIN ANALYZE SELECT song_fk, hash, offset2, (hash_gist <-> 'a3272361800000000000') AS dst FROM Fingerprint_1002156 ORDER BY dst;
