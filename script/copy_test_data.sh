#!/bin/bash

set -e

readonly SOURCE_DATA_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/data"
readonly TARGET_DATA_DIRECTORY="/home/postgres"

function copy_file() {
  cp "${SOURCE_DATA_DIRECTORY}/$1" "${TARGET_DATA_DIRECTORY}/$1"
  chown postgres "${TARGET_DATA_DIRECTORY}/$1"
}

copy_file "1000.csv"
copy_file "fingerprint_create_indexes.sql"
copy_file "fingerprint_create_tables.sql"
copy_file "fingerprint_data_10"
copy_file "fingerprint_data_1000"
copy_file "fingerprint_data_10000"
copy_file "fingerprint_data_1002156"
copy_file "fingerprint_measure.sql"
