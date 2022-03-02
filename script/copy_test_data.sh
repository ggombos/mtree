#!/bin/bash

set -e

readonly SOURCE_SCRIPT_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/script"
readonly SOURCE_DATA_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/data"
readonly TARGET_DATA_DIRECTORY="/home/postgres/test_files"

readonly DATA_FILENAMES=(
  "1000_create_indexes.sql"
  "1000_create_tables.sql"
  "1000_measure.sql"
  "1000.csv"
  "1000_float.csv"
  "2000_int.csv"
  "fingerprint_create_indexes.sql"
  "fingerprint_create_tables.sql"
  "fingerprint_data_10"
  "fingerprint_data_1000"
  "fingerprint_data_10000"
  "fingerprint_data_1002156"
  "fingerprint_measure.sql"
  "int8_measure.sql"
  "float_measure.sql"
  "mtreeOrderbyhiba.sql"
)

readonly SCRIPT_FILENAMES=(
  "start.sh"
  "stop.sh"
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
