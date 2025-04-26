# mtree_gist

This repository contains the data and source code for a new M-tree based index structure implemented for the *PostgreSQL* RDBMS. The implementation is discussed and evaluated in two academic research papers:

[:book:&nbsp;__An implementation of the M-tree index structure for PostgreSQL using GiST__](https://ieeexplore.ieee.org/document/9119265)

[:book:&nbsp;__Towards on experimental comparison of the M-tree index structure with BK-tree and VP-tree__](http://www.aei.tuke.sk/papers/2020/2/03_Gombos.pdf)

This repository was developed based on the official *PostgreSQL* documentation on *GiST extensibility*:
https://www.postgresql.org/docs/15/gist-extensibility.html

## Table of contents

- [Repository structure](#repository-structure)
- [How to Use](#how-to-use)
    - [Installing the Extension](#installing-the-extension)
    - [Using the Extension](#using-the-extension)
    - [Example Query](#example-query)
- [Additional Notes](#additional-notes)

## Repository structure

- `.github/workflows`: Contains the GitHub CI configuration files.
- `docker`: Contains the Dockerfile used for CI builds.
- `source`: Contains the core implementation of the index, written in C and SQL.
- `tests`: Contains data generation script and unit tests. The unit tests are executed during CI.

## How to Use

Before using the index, you need to install it.

*Note*: The installation process is designed for Linux environments.

### Installing the Extension

To compile and install the extension:

```sh
cd source
mkdir build
cd build
cmake ..
make
make install
```

During the installation process the `mtree_gist.so` is copied into the `/usr/share/postgresql/15/postgre/` folder. Furthermore, `mtree_gist--1.0.sql`, `mtree_gist.control` files are copied into the `/usr/share/postgresql/15/extension/` folder. Once these files are in place, the extension is ready to be used.

### Using the Extension

After installation, you must create the extension in *PostgreSQL*:

```sql
DROP EXTENSION IF EXISTS mtree_gist;
CREATE EXTENSION mtree_gist;
```

After this step, *PostgreSQL* will recognize and load the `mtree_gist` extension.

### Example Query

Here's an example showing how to create a table with an mtree column, load data, and create an index:

```sql
CREATE TABLE public.kitchen_mtree (
    id serial primary key,
    point mtree_int32_array
);

COPY public.kitchen_mtree (point) FROM 'tests/int32_array/int32_array_1000_mtree.csv' DELIMITER '''' CSV;

CREATE INDEX kitchen_mtree_index ON public.kitchen_mtree USING gist (
    point gist_mtree_int32_array_ops (
        picksplit_strategy    = 'SamplingMinOverlapArea',
        union_strategy        = 'MinMaxDistance'
    )
);
```

After that, the index is ready to use. For example, you can perform a K-Nearest Neighbors (KNN) query like this:

```sql
SET enable_seqscan TO OFF;
SELECT c.id, c.point, (c.point <-> (SELECT ic.point FROM public.kitchen_mtree ic WHERE ic.id = 1)) dist
FROM public.kitchen_mtree c
ORDER BY c.point <-> (SELECT ic.point FROM public.kitchen_mtree ic WHERE ic.id = 1) LIMIT 10;
```

## Additional Notes

This section contains helpful insights and important findings from our experience developing the *M-tree GiST extension* for *PostgreSQL*.

**consistent**

The `recheck` indicates whether the predicate is certainly true or only possibly true. If `recheck = false` then the index has tested the predicate condition exactly, whereas if `recheck = true` the row is only a candidate match. In that case the system will automatically evaluate the indexable operator against the actual row value to see if it is really a match. This convention allows *GiST* to support both lossless and lossy index structures.

**integer_ops**

If we use `integer_ops` for `ORDER BY` at the ordering operator, *PostgreSQL* will
use sequential scan instead of using our index.

Related: https://www.postgresql.org/docs/15/xindex.html#XINDEX-ORDERING-OPS
