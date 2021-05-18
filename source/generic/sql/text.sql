DROP TABLE IF EXISTS Songs;
CREATE TABLE Songs (data text);

COPY Songs(data) FROM '%datacsv%' DELIMITER ',' CSV HEADER;

SET enable_seqscan = ON;

SELECT COUNT(*) FROM Songs WHERE data #<# '10|efedcbc';
SELECT COUNT(*) FROM Songs WHERE data #<# '10|cdefgab';
SELECT COUNT(*) FROM Songs WHERE data #<# '10|aedecba';
SELECT COUNT(*) FROM Songs WHERE data #<# '10|cffcgef';
SELECT COUNT(*) FROM Songs WHERE data #<# '10|cgagfedc';

CREATE INDEX mtree_test_index_text ON Songs USING gist (data);

SET enable_seqscan = OFF;

SELECT COUNT(*) FROM Songs WHERE data #<# '10|efedcbc'::text;
SELECT COUNT(*) FROM Songs WHERE data #<# '10|cdefgab'::text;
SELECT COUNT(*) FROM Songs WHERE data #<# '10|aedecba'::text;
SELECT COUNT(*) FROM Songs WHERE data #<# '10|cffcgef'::text;
SELECT COUNT(*) FROM Songs WHERE data #<# '10|cgagfedc'::text;
