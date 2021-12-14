\timing off

\echo '#START :: Create table'
DROP TABLE IF EXISTS Songs;
CREATE TABLE Songs (
    x    INTEGER,
    data mtree_text
);
COPY Songs(x, data) FROM '/home/postgres/1000.csv' DELIMITER ',' CSV HEADER;
\echo '#END   :: Create table'

SET enable_seqscan = off;

\timing on

\echo '#START :: Create index'
CREATE INDEX index_test ON Songs USING gist (data mtree_text_opclass);
\echo '#END   :: Create index'

\echo '#START :: Refactored :: Range based query (short)'
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'efedchc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'efedchc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cdefgah';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cdefgah';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'aedecha';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'aedecha';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cffcgef';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cffcgef';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cgagfedc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cgagfedc';
\echo '#END   :: Refactored :: Range based query (short)'

\echo '#START :: Refactored :: Range based query (long)'
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'ceceggcbagfagfedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'ceceggcbagfagfedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cdefedecdefgggegfedecdedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'cdefedecdefgggegfedecdedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'edcdeeedddeffedcdeeededc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'edcdeeedddeffedcdeeededc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'ggagcbggagdcgggeccbaffecdc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'ggagcbggagdcgggeccbaffecdc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'gaggaggagfedefedc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# 'gaggaggagfedefedc';
\echo '#END   :: Refactored :: Range based query (long)'

\echo '#region K Nearest Neighbors - short - few:'
EXPLAIN ANALYZE SELECT x, data, (data <-> 'efedcbc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'efedcbc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefgab') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefgab') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'aedecba') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'aedecba') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cffcgef') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cffcgef') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cgagfedc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cgagfedc') AS dst FROM Songs ORDER BY dst LIMIT 10;
\echo '#endregion'

\echo '#region K Nearest Neighbors - short - many:'
EXPLAIN ANALYZE SELECT x, data, (data <-> 'efedcbc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'efedcbc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefgab') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefgab') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'aedecba') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'aedecba') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cffcgef') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cffcgef') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cgagfedc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cgagfedc') AS dst FROM Songs ORDER BY dst LIMIT 100;
\echo '#endregion'

\echo '#region K Nearest Neighbors - long - few:'
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ceceggcbagfagfedcc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ceceggcbagfagfedcc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefedecdefgggegfedecdedcc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefedecdefgggegfedecdedcc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'edcdeeedddeffedcdeeededc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'edcdeeedddeffedcdeeededc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ggagcbggagdcgggeccbaffecdc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ggagcbggagdcgggeccbaffecdc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'gaggaggagfedefedc') AS dst FROM Songs ORDER BY dst LIMIT 10;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'gaggaggagfedefedc') AS dst FROM Songs ORDER BY dst LIMIT 10;
\echo '#endregion'

\echo '#region K Nearest Neighbors - long - many:'
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ceceggcbagfagfedcc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ceceggcbagfagfedcc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefedecdefgggegfedecdedcc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'cdefedecdefgggegfedecdedcc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'edcdeeedddeffedcdeeededc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'edcdeeedddeffedcdeeededc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ggagcbggagdcgggeccbaffecdc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'ggagcbggagdcgggeccbaffecdc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'gaggaggagfedefedc') AS dst FROM Songs ORDER BY dst LIMIT 100;
EXPLAIN ANALYZE SELECT x, data, (data <-> 'gaggaggagfedefedc') AS dst FROM Songs ORDER BY dst LIMIT 100;
\echo '#endregion'

\echo '#START :: Drop index'
DROP INDEX index_test;
\echo '#END   :: Drop index'

\echo '#START :: Drop table if exists'
DROP TABLE IF EXISTS Songs;
\echo '#END   :: Drop table if exists'
