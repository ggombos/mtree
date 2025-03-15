#!/bin/bash

set -e

# Source code directory of the M-tree index
readonly SOURCE_DIRECTORY="/home/data/mtree/source"
# Include directory
# readonly POSTGRESQL_INCLUDE_DIRECTORY="/home/data/mtree_gist/postgre/include/postgresql/server"
readonly POSTGRESQL_INCLUDE_DIRECTORY="/usr/include/postgresql/15/server"
# PostgreSQL home of SQL and control files
readonly POSTGRESQL_EXTENSION_DIRECTORY="/usr/share/postgresql/15/extension"
# PostgreSQL home of shared object libraries
readonly POSTGRESQL_LIBRARY_DIRECTORY="/usr/share/postgresql/15/postgre"

readonly FILENAMES=(
  "mtree_text"
  "mtree_text_util"
  "mtree_text_array"
  "mtree_text_array_util"
  "mtree_int32"
  "mtree_int32_util"
  "mtree_int32_array"
  "mtree_int32_array_util"
  "mtree_float"
  "mtree_float_util"
  "mtree_float_array"
  "mtree_float_array_util"
  "mtree_util"
  "mtree_gist"
)

function compile_file() {
  cc -Wall -Werror -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/$1.c" -o "${SOURCE_DIRECTORY}/$1.o"
}

function remove_file() {
  rm "${SOURCE_DIRECTORY}/$1.o"
}

function create_parameter_list() {
  local parameter_list=""
  for filename in "${FILENAMES[@]}";
  do
    parameter_list+=" ${SOURCE_DIRECTORY}/${filename}.o"
  done
  echo "${parameter_list}"
}

function create_and_copy_so() {
  cc -shared -o "${SOURCE_DIRECTORY}/mtree_gist.so" $(create_parameter_list)
  cp "${SOURCE_DIRECTORY}/mtree_gist.so" "${POSTGRESQL_LIBRARY_DIRECTORY}/mtree_gist.so"
  rm "${SOURCE_DIRECTORY}/mtree_gist.so"
}

function copy_sql_and_control() {
  cp "${SOURCE_DIRECTORY}/mtree_gist--1.0.sql" "${POSTGRESQL_EXTENSION_DIRECTORY}/mtree_gist--1.0.sql"
  cp "${SOURCE_DIRECTORY}/mtree_gist.control" "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
  sed -i 's,%libdir%,'"${POSTGRESQL_LIBRARY_DIRECTORY}"',g' "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
  cp "${SOURCE_DIRECTORY}/mtree_gist_tmp.control" "${POSTGRESQL_EXTENSION_DIRECTORY}/mtree_gist.control"
  rm "${SOURCE_DIRECTORY}/mtree_gist_tmp.control"
}

function createSQL() {
	cat ../source/mtree_gist--1.0.empty.sql > ../source/mtree_gist--1.0.sql
	for filename in "../source/typesSQL"/*
	do
		cat "${filename}" >> ../source/mtree_gist--1.0.sql
	done	
}

# createSQL

for filename in "${FILENAMES[@]}";
do
  compile_file "${filename}"
done

create_and_copy_so

for filename in "${FILENAMES[@]}";
do
  remove_file "${filename}"
done

copy_sql_and_control
