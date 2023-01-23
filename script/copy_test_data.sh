#!/bin/bash

set -e

readonly PROPERTIES_FILE="./script/mtree_gist.properties"

function read_property() {
  PROPERTY_KEY=$1
  PROPERTY_VALUE=$(cat ${PROPERTIES_FILE} | grep "${PROPERTY_KEY}" | cut -d '=' -f 2 | tr -d '\n')
  echo "${PROPERTY_VALUE}"
}

SOURCE_SCRIPT_DIRECTORY="$(read_property "mtree.source")/script"
SOURCE_DATA_DIRECTORY="$(read_property "mtree.source")/data"
TARGET_DATA_DIRECTORY="$(read_property "postgresql.test")"

readonly DATA_FILENAMES=(
  "float4_array.csv"
  "float4_array.expected"
  "float4_array.sql"

  "float4.csv"
  "float4.expected"
  "float4.sql"

  "int8_array.csv"
  "int8_array.expected"
  "int8_array.sql"

  "int8.csv"
  "int8.expected"
  "int8.sql"

  "text_array.csv"
  "text_array.expected"
  "text_array.sql"

  "text.csv"
  "text.expected"
  "text.sql"
  "text_use_index.sql"

  "setup.sql"
  "teardown.sql"

  "MillionSongsDataset.csv"
)

readonly SCRIPT_FILENAMES=(
  "mtree_gist.properties"
  "start.sh"
  "stop.sh"
  "test.sh"
)

function copy_file() {
  cp "$1/$2" "${TARGET_DATA_DIRECTORY}/$2"
  chown postgres "${TARGET_DATA_DIRECTORY}/$2"
}

function copy_data_file() {
  copy_file "${SOURCE_DATA_DIRECTORY}" "$1"
}

function copy_script_file() {
  copy_file "${SOURCE_SCRIPT_DIRECTORY}" "$1"
}

for filename in "${DATA_FILENAMES[@]}";
do
  copy_data_file "${filename}"
done

for filename in "${SCRIPT_FILENAMES[@]}";
do
  copy_script_file "${filename}"
done
