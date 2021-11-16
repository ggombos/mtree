# NOTES

This document contains useful information about GiST extension development which
we discovered while creating the M-tree index extension.

## mtree_gist--1.0.sql

### Operator class

#### Function numbers

The function numbers do matter in an operator class.

Related:

https://www.postgresql.org/docs/13/xindex.html#XINDEX-GIST-SUPPORT-TABLE

#### integer_ops

If we use `integer_ops` for `ORDER BY` at the ordering operator, PostgreSQL will
use sequential scan instead of using our index.

Related:

https://www.postgresql.org/docs/13/xindex.html#XINDEX-ORDERING-OPS
