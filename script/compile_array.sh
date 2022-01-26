#!/bin/bash

set -e

readonly SOURCE_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/source"
readonly POSTGRESQL_DIRECTORY="/usr/include/postgresql/server"

function compile_source_code() {
  cp "$1" "$2"
  gcc -fPIC -c -isystem "${POSTGRESQL_DIRECTORY}" "$2" -o "$3" -Wall -Wextra -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wconversion -Wunreachable-code
  rm "$2"
  rm "$3"
}

clear
compile_source_code "${SOURCE_DIRECTORY}/mtree_int8_array.c" "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.c" "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.o"
