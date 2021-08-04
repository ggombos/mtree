# M-tree index extension for PostgreSQL

## Motivation

The idea was described in an academic research written by *Gergő Gombos*, *János Márk Szalai-Gindl*, *István Donkó* and *Attila Kiss*: **[Towards on experimental comparison of the M-tree index structure with BK-tree and VP-tree](https://www.researchgate.net/publication/343391245_TOWARDS_ON_EXPERIMENTAL_COMPARISON_OF_THE_M-TREE_INDEX_STRUCTURE_WITH_BK-TREE_AND_VP-TREE)**.

The paper was published in **2020** at *Acta Electrotechnica et Informatica*.

### Other index implementations

The original VP-tree and BK-tree implementations can be found [HERE](https://github.com/fake-name/pg-spgist_hamming).

## How to use

To make life easier, I created several `bash` scripts to make build and installation faster. In the following I will explain how to use them.
### How to try M-tree index

Change directory to the root folder with the normal user (zsolt).

```sh
cd /
```

Install M-tree index from source.

```sh
sudo bash /run/media/zsolt/DATA/Development/mtree_gist/script/install
```

Start server.

```sh
sudo bash /run/media/zsolt/DATA/Development/mtree_gist/script/start
```

Change to database user (postgres).

```sh
su postgres
```

Get into PostgreSQL interactive terminal.

```sh
psql
```

Create extension.

```sh
CREATE INDEX index_test ON Songs USING gist (data gist_text_ops);
```

Measure performance.

```sh
\i /home/postgres/measure.sql
```

Drop extension.

```sh
DROP EXTENSION mtree_gist;
```

Get out of PostgreSQL interactive terminal.

```sh
exit
```

Change to normal user (zsolt).

```sh
su zsolt
```

Stop server.

```sh
sudo bash /run/media/zsolt/DATA/Development/mtree_gist/script/stop
```
