#!/bin/bash

set -e

readonly SOURCE_FILE="/home/zsolt/DATA/Development/mtree_gist/source/mtree_text.c"
readonly SOURCE_FILE_TMP="/home/zsolt/DATA/Development/mtree_gist/source/mtree_tmp.c"
readonly OBJECT_FILE="/home/zsolt/DATA/Development/mtree_gist/source/mtree_tmp.o"
readonly POSTGRESQL_DIRECTORY="/usr/include/postgresql/server"

function compile_source_code() {
  cp "${SOURCE_FILE}" "${SOURCE_FILE_TMP}"
  gcc -fPIC -c -isystem "${POSTGRESQL_DIRECTORY}" "${SOURCE_FILE_TMP}" -o "${OBJECT_FILE}" -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
  rm "${SOURCE_FILE_TMP}"
  rm "${OBJECT_FILE}"
}

clear
compile_source_code
