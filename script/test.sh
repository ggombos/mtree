#!/bin/bash

set -e

readonly TEST_DIRECTORY="/home/postgres/test_files"

readonly TEST_TYPES=(
  "float4_array"
  "float4"
#  "int8_array"
  "int8"
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
  echo "All $1 tests were successful!"
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
done
teardown
