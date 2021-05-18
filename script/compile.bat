@echo off

cls

set SOURCE_FILE=e:\Development\mtree\source\generic\mtree_text.c
set SOURCE_FILE_TMP=e:\Development\mtree\source\generic\mtree_tmp.c
set SOURCE_FILE_OBJECT=e:\Development\mtree\source\generic\mtree_tmp.o
set POSTGRESQL_DIRECTORY=e:\Development\c_include\server

xcopy %SOURCE_FILE% %SOURCE_FILE_TMP% /I /Y
gcc -fPIC -c -isystem %POSTGRESQL_DIRECTORY% %SOURCE_FILE_TMP% -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
del %SOURCE_FILE_TMP%
del %SOURCE_FILE_OBJECT%
