#!/bin/bash

set -e

readonly SOURCE_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/source"
readonly POSTGRESQL_INCLUDE_DIRECTORY="/usr/include/postgresql/server"
readonly POSTGRESQL_EXTENSION_DIRECTORY="/usr/share/postgresql/extension"
readonly POSTGRESQL_LIBRARY_DIRECTORY="/usr/lib/postgresql"

function create_and_copy_so() {
  cp "${SOURCE_DIRECTORY}/mtree_text.c" "${SOURCE_DIRECTORY}/mtree_text_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text.o"
  cp "${SOURCE_DIRECTORY}/mtree_text_util.c" "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_int8.c" "${SOURCE_DIRECTORY}/mtree_int8_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8.o"
  cp "${SOURCE_DIRECTORY}/mtree_int8_util.c" "${SOURCE_DIRECTORY}/mtree_int8_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_util.o"

  cp "${SOURCE_DIRECTORY}/mtree_int8_array.c" "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array.o"
  cp "${SOURCE_DIRECTORY}/mtree_int8_array_util.c" "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"
  #cp "${SOURCE_DIRECTORY}/mtree_int8_array_util.c" "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c"
  #cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_util.c" "${SOURCE_DIRECTORY}/mtree_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_gist.c" "${SOURCE_DIRECTORY}/mtree_gist_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_gist_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_gist.o"
  #cc -shared -o "${SOURCE_DIRECTORY}/mtree_int8.so" "${SOURCE_DIRECTORY}/mtree_int8.o" "${SOURCE_DIRECTORY}/mtree_int8_util.o" "${SOURCE_DIRECTORY}/mtree_util.o" "${SOURCE_DIRECTORY}/mtree_gist.o"
  cc -shared -o "${SOURCE_DIRECTORY}/mtree_int8_array.so" "${SOURCE_DIRECTORY}/mtree_int8_array.o" "${SOURCE_DIRECTORY}/mtree_int8_array_util.o" "${SOURCE_DIRECTORY}/mtree_gist.o" "${SOURCE_DIRECTORY}/mtree_text.o" "${SOURCE_DIRECTORY}/mtree_text_util.o" "${SOURCE_DIRECTORY}/mtree_int8.o" "${SOURCE_DIRECTORY}/mtree_int8_util.o" "${SOURCE_DIRECTORY}/mtree_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_int8_array.so" "${POSTGRESQL_LIBRARY_DIRECTORY}/mtree_gist.so"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_gist_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array.so"
  rm "${SOURCE_DIRECTORY}/mtree_gist.o"

  rm "${SOURCE_DIRECTORY}/mtree_text_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_int8_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_int8_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_text.o"
  rm "${SOURCE_DIRECTORY}/mtree_text_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_util.o"
}

function copy_sql_and_control() {
  cp "${SOURCE_DIRECTORY}/mtree_gist--1.0.sql" "${POSTGRESQL_EXTENSION_DIRECTORY}/mtree_gist--1.0.sql"
  cp "${SOURCE_DIRECTORY}/mtree_gist.control" "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
  sed -i 's,%libdir%,'"${POSTGRESQL_LIBRARY_DIRECTORY}"',g' "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
  cp "${SOURCE_DIRECTORY}/mtree_gist_tmp.control" "${POSTGRESQL_EXTENSION_DIRECTORY}/mtree_gist.control"
  rm "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
}

create_and_copy_so
copy_sql_and_control

readonly SOURCE_DATA_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/data"
readonly TARGET_DATA_DIRECTORY="/home/postgres"

function copy_file() {
  cp "$1/$2" "${TARGET_DATA_DIRECTORY}/$2"
  chown postgres "${TARGET_DATA_DIRECTORY}/$2"
}

copy_file "${SOURCE_DATA_DIRECTORY}" "array_test.csv"
copy_file "${SOURCE_DATA_DIRECTORY}" "array_test.sql"
