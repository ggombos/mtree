#!/bin/bash

set -e

readonly SOURCE_SCRIPT_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/script"
readonly SOURCE_DATA_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/data"
readonly TARGET_DATA_DIRECTORY="/home/postgres/test_files"

readonly DATA_FILENAMES=(
  "float4.csv"
  "float4.expected"
  "float4.sql"

  "float4_array.csv"
  "float4_array.expected"
  "float4_array.sql"

  "int8.csv"
  "int8.expected"
  "int8.sql"

  "int8_array.csv"
  "int8_array.expected"
  "int8_array.sql"

  "text.csv"
  "text.expected"
  "text.sql"

  "text_array.csv"
  "text_array.expected"
  "text_array.sql"

  "setup.sql"
  "teardown.sql"
)

readonly SCRIPT_FILENAMES=(
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
