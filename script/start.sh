#!/bin/bash

set -e

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

su postgres -c "pg_ctl start --pgdata=/home/postgres/data --log=/home/postgres/server.log"
