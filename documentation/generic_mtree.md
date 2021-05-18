# Generic M-tree index in PostgreSQL

This is the documentation of the generalized M-tree index in PostgreSQL.
This implementation is based on the official B-tree index implementation.

## Our target

We want to implement an M-tree based index extension supporting various data types.

## File contents

In the following we explain the source file contents.

### `mtree_gist--1.0.sql`

Creates the SQL types, functions and operators (operator families) to support GiST implementation of the index.

### `mtree_gist.h`

Declares all the supported types.
List of all supported types:

- `int2`
- `text`

### `mtree_gist.c`

Implements the generalized `input`, `output` and `decompress` methods.
In our case, decompress doesn't do anything useful.

### `mtree_utils_var.h`

Declares several types to implement variable length data structures with the help of `varlena` data type. The code often referst to it by the `bytea` alias.

### `mtree_utils_var.c`



### `mtree_text.c`


