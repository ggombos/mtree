DROP INDEX IF EXISTS Test_1000_GiST_Mtree CASCADE;
CREATE INDEX Test_1000_GiST_Mtree ON Test_1000 USING GiST (data mtree_text_opclass);
