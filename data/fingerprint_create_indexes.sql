DROP INDEX IF EXISTS Fingerprint_1000_GiST_Mtree CASCADE;
CREATE INDEX Fingerprint_1000_GiST_Mtree ON Fingerprint_1000 USING GiST (hash_gist mtree_text_opclass);

DROP INDEX IF EXISTS Fingerprint_10000_GiST_Mtree CASCADE;
CREATE INDEX Fingerprint_10000_GiST_Mtree ON Fingerprint_10000 USING GiST (hash_gist mtree_text_opclass);

DROP INDEX IF EXISTS Fingerprint_1002156_GiST_Mtree CASCADE;
CREATE INDEX Fingerprint_1002156_GiST_Mtree ON Fingerprint_1002156 USING GiST (hash_gist mtree_text_opclass);
