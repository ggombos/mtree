#!/bin/bash
#
# Compiles M-tree implementation with all useful flags.

readonly SOURCE_FILE="/home/zsolt/WD/Development/mtree_gist/source/generic/mtree_text.c"
readonly SOURCE_FILE_TMP="/home/zsolt/WD/Development/mtree_gist/source/generic/mtree_tmp.c"
readonly SOURCE_FILE_OBJECT="./mtree_tmp.o"
readonly POSTGRESQL_DIRECTORY="/usr/local/pgsql/include/server"

#######################################
# Compiles C source code with flags.
# Arguments:
#   None
#######################################
function compile_source_code() {
  cp "${SOURCE_FILE}" "${SOURCE_FILE_TMP}"
  sed -i 's,%picksplitStrategy%,'"Random"',g' "${SOURCE_FILE_TMP}"
  gcc -fPIC -c -isystem "${POSTGRESQL_DIRECTORY}" "${SOURCE_FILE_TMP}" -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
  rm "${SOURCE_FILE_TMP}"
  rm "${SOURCE_FILE_OBJECT}"
}

clear
compile_source_code
