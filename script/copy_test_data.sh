#!/bin/bash

set -e

readonly SOURCE_SCRIPT_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/script"
readonly SOURCE_DATA_DIRECTORY="/home/zsolt/DATA/Development/mtree_gist/data"
readonly TARGET_DATA_DIRECTORY="/home/postgres"

function copy_file() {
  cp "$1/$2" "${TARGET_DATA_DIRECTORY}/$2"
  chown postgres "${TARGET_DATA_DIRECTORY}/$2"
}

copy_file "${SOURCE_SCRIPT_DIRECTORY}" "start.sh"
copy_file "${SOURCE_SCRIPT_DIRECTORY}" "stop.sh"

copy_file "${SOURCE_DATA_DIRECTORY}" "1000_create_indexes.sql"
copy_file "${SOURCE_DATA_DIRECTORY}" "1000_create_tables.sql"
copy_file "${SOURCE_DATA_DIRECTORY}" "1000_measure.sql"
copy_file "${SOURCE_DATA_DIRECTORY}" "1000.csv"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_create_indexes.sql"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_create_tables.sql"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_data_10"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_data_1000"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_data_10000"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_data_1002156"
copy_file "${SOURCE_DATA_DIRECTORY}" "fingerprint_measure.sql"
copy_file "${SOURCE_DATA_DIRECTORY}" "mtreeOrderbyhiba.sql"
