# M-tree index extension for PostgreSQL

## Motivation

The idea was described in an academic research written by *Gergő Gombos*, *János Márk Szalai-Gindl*, *István Donkó* and *Attila Kiss*: **[Towards on experimental comparison of the M-tree index structure with BK-tree and VP-tree](https://www.researchgate.net/publication/343391245_TOWARDS_ON_EXPERIMENTAL_COMPARISON_OF_THE_M-TREE_INDEX_STRUCTURE_WITH_BK-TREE_AND_VP-TREE)**.

The paper was published in **2020** at *Acta Electrotechnica et Informatica*.

### Other index implementations

The original VP-tree and BK-tree implementations can be found [HERE](https://github.com/fake-name/pg-spgist_hamming).

## How to use

To make life easier, I created several `bash` scripts to make build and installation faster. In the following I will explain how to use them. You will have to go through the source code of the `install.sh`, `start.sh` and `stop.sh` scripts to change the hard coded absolute paths.

### How to try M-tree index

Run the following commands with your user:

```bash
$ git clone https://github.com/sajtizsolt/mtree_gist
$ cd mtree_gist
$ sudo bash script/install.sh
```

Note, that to run the following commands, you will need a database user on your machine - I will call this user `postgres`. You will also need to have PostgreSQL installed. Open another terminal window and run the following commands:

```bash
$ sudo -iu postgres
$ sudo script/stop.sh
$ sudo script/start.sh
$ psql
```

At this point your terminal will change, because you will use the `PostgreSQL` interactive terminal. Please note, that the `postgres` user should own the specified file.

```sql
> DROP EXTENSION IF EXISTS mtree_gist CASCADE;
> CREATE EXTENSION mtree_gist;
> \i <absolute-path-to-sql-file>
> DROP EXTENSION mtree_gist CASCADE;
> exit
```

## Development

Read [this](documentation/DEVELOPMENT.md) document for some development notes.

Read [this](documentation/TODO.md) document for the to-do list.
