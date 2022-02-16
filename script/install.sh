#!/bin/bash

set -e

readonly SOURCE_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/source"
readonly POSTGRESQL_INCLUDE_DIRECTORY="/usr/include/postgresql/server"
readonly POSTGRESQL_EXTENSION_DIRECTORY="/usr/share/postgresql/extension"
readonly POSTGRESQL_LIBRARY_DIRECTORY="/usr/lib/postgresql"

function create_and_copy_so() {
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text.c" -o "${SOURCE_DIRECTORY}/mtree_text.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_util.c" -o "${SOURCE_DIRECTORY}/mtree_text_util.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8.c" -o "${SOURCE_DIRECTORY}/mtree_int8.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_util.c" -o "${SOURCE_DIRECTORY}/mtree_int8_util.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array_util.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_array.c" -o "${SOURCE_DIRECTORY}/mtree_text_array.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_array_util.c" -o "${SOURCE_DIRECTORY}/mtree_text_array_util.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_util.c" -o "${SOURCE_DIRECTORY}/mtree_util.o"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_gist.c" -o "${SOURCE_DIRECTORY}/mtree_gist.o"
  cc -shared -o "${SOURCE_DIRECTORY}/mtree_gist.so" "${SOURCE_DIRECTORY}/mtree_gist.o" "${SOURCE_DIRECTORY}/mtree_text.o" "${SOURCE_DIRECTORY}/mtree_text_util.o" "${SOURCE_DIRECTORY}/mtree_int8.o" "${SOURCE_DIRECTORY}/mtree_int8_util.o" "${SOURCE_DIRECTORY}/mtree_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_gist.so" "${POSTGRESQL_LIBRARY_DIRECTORY}/mtree_gist.so"
  rm "${SOURCE_DIRECTORY}/mtree_text.o"
  rm "${SOURCE_DIRECTORY}/mtree_text_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_text_array.o"
  rm "${SOURCE_DIRECTORY}/mtree_text_array_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_gist.o"
  rm "${SOURCE_DIRECTORY}/mtree_gist.so"
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
