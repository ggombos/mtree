#!/bin/bash
#
# Starts the database server.

# Create lock file directory
if [[ ! -d "/run/postgresql" ]]
then
  mkdir /run/postgresql
  chown postgres /run/postgresql
fi

# Create history file
if [[ ! -f "/var/lib/postgres/.psql_history" ]]
then
  touch /var/lib/postgres/.psql_history
  chown postgres /var/lib/postgres/.psql_history
fi

# Start server
su postgres -c "pg_ctl start --pgdata=/usr/local/pgsql/data --log=/home/postgres/server.log"
