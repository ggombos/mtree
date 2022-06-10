#!/bin/bash

set -e

readonly PROPERTIES_FILE="./test_files/mtree_gist.properties"

function read_property() {
  PROPERTY_KEY=$1
  PROPERTY_VALUE=$(cat ${PROPERTIES_FILE} | grep "${PROPERTY_KEY}" | cut -d '=' -f 2 | tr -d '\n')
  echo "${PROPERTY_VALUE}"
}

TEST_DIRECTORY="$(read_property "postgresql.test")"

readonly TEST_TYPES=(
#  "float4_array"
#  "float4"
#  "int8_array"
#  "int8"
#  "text_array"
  "text"
)

function setup() {
  echo ""
  echo "--------------------< setup >--------------------"
  psql --username=postgres --file="${TEST_DIRECTORY}/setup.sql"
}

function run_test() {
  echo ""
  echo "--------------------< $1 >--------------------"
  psql --username=postgres --file="${TEST_DIRECTORY}/$1.sql" --output="${TEST_DIRECTORY}/$1.output" --echo-all
  echo ""
  echo "$1 test results:"
  cmp "${TEST_DIRECTORY}/$1.expected" "${TEST_DIRECTORY}/$1.output"
  echo "$1 tests were successful!"
}

function run_test_use_index() {
  psql --username=postgres --file="${TEST_DIRECTORY}/$1_use_index.sql" --output="${TEST_DIRECTORY}/$1_use_index.output" --echo-all
  echo ""
  echo "$1_use_index test results:"
  grep "${TEST_DIRECTORY}/$1_use_index.output" -e "Index Scan using mtree_$1_test_idx on mtree_$1_test"
  echo "$1_use_index tests were successful!"
}

function teardown() {
  echo ""
  echo "--------------------< teardown >--------------------"
  psql --username=postgres --file="${TEST_DIRECTORY}/teardown.sql"
  echo ""
  echo "All tests were successful!"
}

setup
for type in "${TEST_TYPES[@]}";
do
  run_test "${type}"
  run_test_use_index "${type}"
done
teardown
