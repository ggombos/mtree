#!/bin/bash

# Switch user
su postgres

# Drop extension if exists
/usr/local/postgresql/bin/psql -c 'DROP EXTENSION IF EXISTS mtree1 CASCADE'

# Create extension
/usr/local/postgresql/bin/psql -c 'CREATE EXTENSION mtree1'

# Measure performance
/usr/local/postgresql/bin/psql -f /usr/local/postgresql/data/measure.sql
