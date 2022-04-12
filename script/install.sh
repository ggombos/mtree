#!/bin/bash

set -e

readonly PROPERTIES_FILE="./script/mtree_gist.properties"

function read_property() {
  PROPERTY_KEY=$1
  PROPERTY_VALUE=$(cat ${PROPERTIES_FILE} | grep "${PROPERTY_KEY}" | cut -d '=' -f 2 | tr -d '\n')
  echo "${PROPERTY_VALUE}"
}

SOURCE_DIRECTORY="$(read_property "mtree.source")/source"
POSTGRESQL_INCLUDE_DIRECTORY="$(read_property "postgresql.include")"
POSTGRESQL_EXTENSION_DIRECTORY="$(read_property "postgresql.extension")"
POSTGRESQL_LIBRARY_DIRECTORY="$(read_property "postgresql.lib")"

readonly FILENAMES=(
  "mtree_text"
  "mtree_text_util"
  "mtree_text_array"
  "mtree_text_array_util"
  "mtree_int8"
  "mtree_int8_util"
  "mtree_int8_array"
  "mtree_int8_array_util"
  "mtree_float"
  "mtree_float_util"
  "mtree_float_array"
  "mtree_float_array_util"
  "mtree_util"
  "mtree_gist"
)

function compile_file() {
  cc -fPIC -c -I "${POSTGRESQL_INCLUDE_DIRECTORY}" "${SOURCE_DIRECTORY}/$1.c" -o "${SOURCE_DIRECTORY}/$1.o"
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
  # shellcheck disable=SC2046
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
