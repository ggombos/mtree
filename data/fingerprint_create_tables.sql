DROP EXTENSION IF EXISTS mtree_gist CASCADE;
CREATE EXTENSION mtree_gist;

DROP TABLE IF EXISTS Fingerprint_10 CASCADE;
CREATE TABLE Fingerprint_10 (
  id        SERIAL PRIMARY KEY,
  song_fk   INTEGER,
  hash	    TEXT,
  offset2   INTEGER,
  hash_gist mtree_text
);
COPY Fingerprint_10 FROM '/home/postgres/fingerprint_data_10';

DROP TABLE IF EXISTS Fingerprint_1000 CASCADE;
CREATE TABLE Fingerprint_1000 (
  id        SERIAL PRIMARY KEY,
  song_fk   INTEGER,
  hash	    TEXT,
  offset2   INTEGER,
  hash_gist mtree_text
);
COPY Fingerprint_1000 FROM '/home/postgres/fingerprint_data_1000';

DROP TABLE IF EXISTS Fingerprint_10000 CASCADE;
CREATE TABLE Fingerprint_10000 (
  id        SERIAL PRIMARY KEY,
  song_fk   INTEGER,
  hash	    TEXT,
  offset2   INTEGER,
  hash_gist mtree_text
);
COPY Fingerprint_10000 FROM '/home/postgres/fingerprint_data_10000';

DROP TABLE IF EXISTS Fingerprint_1002156 CASCADE;
CREATE TABLE Fingerprint_1002156 (
  id        SERIAL PRIMARY KEY,
  song_fk   INTEGER,
  hash	    TEXT,
  offset2   INTEGER,
  hash_gist mtree_text
);
COPY Fingerprint_1002156 FROM '/home/postgres/fingerprint_data_1002156';
