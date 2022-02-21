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

function create_and_copy_so() {
  # text
  cp "${SOURCE_DIRECTORY}/mtree_text.c" "${SOURCE_DIRECTORY}/mtree_text_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text.o"
  cp "${SOURCE_DIRECTORY}/mtree_text_util.c" "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text_util.o"
  
  #int8
  cp "${SOURCE_DIRECTORY}/mtree_int8.c" "${SOURCE_DIRECTORY}/mtree_int8_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8.o"
  cp "${SOURCE_DIRECTORY}/mtree_int8_util.c" "${SOURCE_DIRECTORY}/mtree_int8_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_util.o"
  
  #util
  cp "${SOURCE_DIRECTORY}/mtree_util.c" "${SOURCE_DIRECTORY}/mtree_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_util.o"
  
  #gist
  cp "${SOURCE_DIRECTORY}/mtree_gist.c" "${SOURCE_DIRECTORY}/mtree_gist_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_gist_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_gist.o"
  
  #int8 array
  cp "${SOURCE_DIRECTORY}/mtree_int8_array.c" "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array.o"
  cp "${SOURCE_DIRECTORY}/mtree_int8_array_util.c" "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_int8_array_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"
  
  #text array
  cp "${SOURCE_DIRECTORY}/mtree_text_array.c" "${SOURCE_DIRECTORY}/mtree_text_array_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_array_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text_array.o"
  cp "${SOURCE_DIRECTORY}/mtree_text_array_util.c" "${SOURCE_DIRECTORY}/mtree_text_array_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_text_array_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_text_array_util.o"
  
  #float array
  cp "${SOURCE_DIRECTORY}/mtree_float_array.c" "${SOURCE_DIRECTORY}/mtree_float_array_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_float_array_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_float_array.o"
  cp "${SOURCE_DIRECTORY}/mtree_float_array_util.c" "${SOURCE_DIRECTORY}/mtree_float_array_util_tmp.c"
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/mtree_float_array_util_tmp.c" -o "${SOURCE_DIRECTORY}/mtree_float_array_util.o"

  # cc -shared -o "${SOURCE_DIRECTORY}/mtree_text.so" "${SOURCE_DIRECTORY}/mtree_gist.o" "${SOURCE_DIRECTORY}/mtree_text.o" "${SOURCE_DIRECTORY}/mtree_text_util.o" "${SOURCE_DIRECTORY}/mtree_int8.o" "${SOURCE_DIRECTORY}/mtree_int8_util.o" "${SOURCE_DIRECTORY}/mtree_util.o"
  # cp "${SOURCE_DIRECTORY}/mtree_text.so" "${POSTGRESQL_LIBRARY_DIRECTORY}/mtree_gist.so"
  
    # cc -shared -o "${SOURCE_DIRECTORY}/mtree.so" "${SOURCE_DIRECTORY}/mtree_text_array.o" "${SOURCE_DIRECTORY}/mtree_text_array_util.o" "${SOURCE_DIRECTORY}/mtree_gist.o" "${SOURCE_DIRECTORY}/mtree_text.o" "${SOURCE_DIRECTORY}/mtree_text_util.o" "${SOURCE_DIRECTORY}/mtree_int8.o" "${SOURCE_DIRECTORY}/mtree_int8_util.o" "${SOURCE_DIRECTORY}/mtree_util.o"
    cc -shared -o "${SOURCE_DIRECTORY}/mtree.so" "${SOURCE_DIRECTORY}/mtree_int8_array.o" "${SOURCE_DIRECTORY}/mtree_int8_array_util.o"  "${SOURCE_DIRECTORY}/mtree_float_array.o" "${SOURCE_DIRECTORY}/mtree_float_array_util.o" "${SOURCE_DIRECTORY}/mtree_text_array.o" "${SOURCE_DIRECTORY}/mtree_text_array_util.o" "${SOURCE_DIRECTORY}/mtree_gist.o" "${SOURCE_DIRECTORY}/mtree_text.o" "${SOURCE_DIRECTORY}/mtree_text_util.o" "${SOURCE_DIRECTORY}/mtree_int8.o" "${SOURCE_DIRECTORY}/mtree_int8_util.o" "${SOURCE_DIRECTORY}/mtree_util.o"
	cp "${SOURCE_DIRECTORY}/mtree.so" "${POSTGRESQL_LIBRARY_DIRECTORY}/mtree_gist.so"
	
  rm "${SOURCE_DIRECTORY}/mtree_text_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_text_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_int8_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_int8_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_util_tmp.c"
  rm "${SOURCE_DIRECTORY}/mtree_text.o"
  rm "${SOURCE_DIRECTORY}/mtree_text_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8.o"
  rm "${SOURCE_DIRECTORY}/mtree_int8_util.o"
  rm "${SOURCE_DIRECTORY}/mtree_util.o"
  # rm "${SOURCE_DIRECTORY}/mtree_text.so"
  rm "${SOURCE_DIRECTORY}/mtree.so"
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
