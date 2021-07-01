\timing off

DROP TABLE IF EXISTS Songs;
CREATE TABLE Songs (
    x    INTEGER,
    data text
);
COPY Songs(x, data) FROM '/home/postgres/1000.csv' DELIMITER ',' CSV HEADER;

SET enable_seqscan = off;

\timing on

\echo '#region Create index:'
CREATE INDEX index_test ON Songs USING gist (data gist_text_ops);
\echo '#endregion'

\echo '#region Range based query - short - small:'
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|efedcbc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|efedcbc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cdefgab';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cdefgab';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|aedecba';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|aedecba';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cffcgef';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cffcgef';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cgagfedc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cgagfedc';
\echo '#endregion'

\echo '#region Range based query - short - big:'
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|efedchc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|efedchc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cdefgah';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cdefgah';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|aedecha';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|aedecha';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cffcgef';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cffcgef';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cgagfedc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cgagfedc';
\echo '#endregion'

\echo '#region Range based query - long - small:'
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|ceceggcbagfagfedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|ceceggcbagfagfedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cdefedecdefgggegfedecdedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|cdefedecdefgggegfedecdedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|edcdeeedddeffedcdeeededc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|edcdeeedddeffedcdeeededc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|ggagcbggagdcgggeccbaffecdc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|ggagcbggagdcgggeccbaffecdc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|gaggaggagfedefedc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '10|gaggaggagfedefedc';
\echo '#endregion'

\echo '#region Range based query - long - big:'
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|ceceggcbagfagfedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|ceceggcbagfagfedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cdefedecdefgggegfedecdedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|cdefedecdefgggegfedecdedcc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|edcdeeedddeffedcdeeededc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|edcdeeedddeffedcdeeededc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|ggagcbggagdcgggeccbaffecdc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|ggagcbggagdcgggeccbaffecdc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|gaggaggagfedefedc';
EXPLAIN ANALYZE SELECT * FROM Songs WHERE data #<# '100|gaggaggagfedefedc';
\echo '#endregion'

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
