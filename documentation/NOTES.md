# NOTES

This document contains useful information about GiST extension development which
we discovered while creating the M-tree index extension.

## C

Related:

https://www.postgresql.org/docs/13/gist-extensibility.html

### consistent

The `recheck` indicates whether the predicate is certainly true or only possibly
true. If `recheck = false` then the index has tested the predicate condition
exactly, whereas if `recheck = true` the row is only a candidate match. In that
case the system will automatically evaluate the indexable operator against the
actual row value to see if it is really a match. This convention allows GiST to
support both lossless and lossy index structures.

## SQL

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
