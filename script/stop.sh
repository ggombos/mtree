#!/bin/bash

set -e

su postgres -c "pg_ctl stop --pgdata=/home/postgres/data --log=/home/postgres/server.log"
