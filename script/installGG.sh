#!/bin/bash
#
# Installs the generic PostgreSQL extension.

# Source code directory of the M-tree index
readonly SOURCE_DIRECTORY="/home/ggombos/mtree/mtree_gist/source"
# Include directory
readonly POSTGRESQL_INCLUDE_DIRECTORY="/home/ggombos/mtree/mtree_gist/postgre/include/postgresql/server"
# PostgreSQL home of SQL and control files
readonly POSTGRESQL_EXTENSION_DIRECTORY="/home/ggombos/mtree/mtree_gist/postgre/share/postgresql/extension"
# PostgreSQL home of shared object libraries
readonly POSTGRESQL_LIBRARY_DIRECTORY="/home/ggombos/mtree/mtree_gist/postgre"

#
# Creates shared object library from the generic source code
#
function create_and_copy_so() {
  cp "${SOURCE_DIRECTORY}/mtree_text.c" "${SOURCE_DIRECTORY}/mtree_text_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text.o"
  cp "${SOURCE_DIRECTORY}/mtree_text_util.c" "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_util.c" "${SOURCE_DIRECTORY}/mtree_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_util.o"
  cc -shared -o "${SOURCE_DIRECTORY}/mtree_text.so" "${SOURCE_DIRECTORY}/mtree_text.o" "${SOURCE_DIRECTORY}/mtree_text_util.o" "${SOURCE_DIRECTORY}/mtree_util.o"
  cp "${SOURCE_DIRECTORY}/mtree_text.so" "${POSTGRESQL_LIBRARY_DIRECTORY}/mtree_gist.so"
  rm "${SOURCE_DIRECTORY}/mtree_text_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_util_tmp.c"  
  rm "${SOURCE_DIRECTORY}/mtree_text.o"
  rm "${SOURCE_DIRECTORY}/mtree_text_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_util.o"  
  rm "${SOURCE_DIRECTORY}/mtree_text.so"
}

#
#
#
function copy_sql_and_control() {
  cp "${SOURCE_DIRECTORY}/mtree_gist--1.0.sql" "${POSTGRESQL_EXTENSION_DIRECTORY}/mtree_gist--1.0.sql"
  cp "${SOURCE_DIRECTORY}/mtree_gist.control" "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
  sed -i 's,%libdir%,'"${POSTGRESQL_LIBRARY_DIRECTORY}"',g' "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
  cp "${SOURCE_DIRECTORY}/mtree_gist_tmp.control" "${POSTGRESQL_EXTENSION_DIRECTORY}/mtree_gist.control"
  rm "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
}

create_and_copy_so
copy_sql_and_control
