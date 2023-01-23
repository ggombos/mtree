#!/bin/bash

set -e

readonly PROPERTIES_FILE="./test_files/mtree_gist.properties"

function read_property() {
  PROPERTY_KEY=$1
  PROPERTY_VALUE=$(cat ${PROPERTIES_FILE} | grep "${PROPERTY_KEY}" | cut -d '=' -f 2 | tr -d '\n')
  echo "${PROPERTY_VALUE}"
}

DATA_DIRECTORY="$(read_property "postgresql.data")"
LOG_FILE="$(read_property "postgresql.server.log")"

if [[ ! -d "/run/postgresql" ]]
then
  mkdir /run/postgresql
  chown postgres /run/postgresql
fi

if [[ ! -f "/var/lib/postgres/.psql_history" ]]
then
  touch /var/lib/postgres/.psql_history
  chown postgres /var/lib/postgres/.psql_history
fi

su postgres -c "pg_ctl start --pgdata=${DATA_DIRECTORY} --log=${LOG_FILE}"
