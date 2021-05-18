


# M-tree index extension for PostgreSQL

## Motivation

The idea was described in an academic research written by *Gergő Gombos*, *János Márk Szalai-Gindl*, *István Donkó* and *Attila Kiss*: **[Towards on experimental comparison of the M-tree index structure with BK-tree and VP-tree](https://www.researchgate.net/publication/343391245_TOWARDS_ON_EXPERIMENTAL_COMPARISON_OF_THE_M-TREE_INDEX_STRUCTURE_WITH_BK-TREE_AND_VP-TREE)**.

The paper was published in **2020** at *Acta Electrotechnica et Informatica*.

### Other index implementations

The original VP-tree and BK-tree implementations can be found [HERE](https://github.com/fake-name/pg-spgist_hamming).

## How to use

To make life easier, I created several `bash` scripts to make build and installation faster. In the following I will explain how to use them.

### TL;DR

```sh
bash script/install-postgresql <target-directory> <version-number>
bash script/install-mtree <source-directory> <postgresql-directory>
bash script/start-server <postgresql-directory>
```

### Install PostgreSQL

To use a PostgreSQL index, you will obviously need a working PostgreSQL installation. You can install PostgreSQL from a package manager, but if you are brave enough, you can run my script which will build the package from source code.

```sh
bash script/install-postgresql <target-directory> <version-number>
```

Arguments:
+ `<target-directory>`: Path of the directory where you want to install PostgreSQL without trailing slash *(e.g. /usr/local/postgresql)*
+ `<version-number>`: The PostgreSQL version number you want to install *(e.g. 13.1)*

### Install M-tree index

If you have a working PostgreSQL installation, you can install M-tree index to it. Just run the following script.

```sh
bash script/install-mtree <source-directory> <postgresql-directory>
```

Arguments:
+ `<source-directory>`: Path of this repository's source directory without trailing slash *(e.g. /home/user/mtree/source)*
+ `<postgresql-directory>`: Path of the directory where you installed PostgreSQL without trailing slash *(e.g. /usr/local/postgresql)*

#### Uninstall M-tree index

If something goes wrong or you no longer want to use our M-tree index implementation, you can simply uninstall it using the following script.

```sh
bash script/uninstall-mtree <postgresql-directory>
```

Arguments:
+ `<postgresql-directory>`: Path of the directory where you installed PostgreSQL without trailing slash *(e.g. /usr/local/postgresql)*

### Start server

After PostgreSQL and M-tree index has been installed its time to run the server and do some intense database related stuff!

```sh
bash script/start-server <postgresql-directory>
```

Arguments:
+ `<postgresql-directory>`: Path of the directory where you installed PostgreSQL without trailing slash *(e.g. /usr/local/postgresql)*

#### Stop server

If you feel that you accomplished everything in your life, you can stop the server with the following script.

```sh
bash script/stop-server <postgresql-directory>
```

Arguments:
+ `<postgresql-directory>`: Path of the directory where you installed PostgreSQL without trailing slash *(e.g. /usr/local/postgresql)*

### Create extensions and measure performance

Using the index is a little bit more complicated so I can't provide scripts for this purpose, but I will show the commands you should use. You can find useful commands in `documentation` folder.

First, you should switch to `postgres` user.

```sh
su postgres
```

After you switched user, you should create the extension you want to try out. If something goes wrong, run these commands from `psql` prompt or from `PGAdmin`.

```sh
<postgresql-directory>/bin/psql -c 'DROP EXTENSION IF EXISTS <extension-name> CASCADE'
<postgresql-directory>/bin/psql -c 'CREATE EXTENSION <extension-name>'
```

After you created the extension, you can run the measuring SQL script.

```sh
<postgresql-directory>/bin/psql -f <postgresql-directory>/data/measure.sql
```

**Profit.**
