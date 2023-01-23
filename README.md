# mtree_gist

This repository contains the data and source code of a new M-tree based index structure for _PostgreSQL_ RDBMS. There are two academic research papers which discusses the usage and efficiency of this implementation.

[:book:&nbsp;__An implementation of the M-tree index structure for PostgreSQL using GiST__](https://ieeexplore.ieee.org/document/9119265)

[:book:&nbsp;__Towards on experimental comparison of the M-tree index structure with BK-tree and VP-tree__](http://www.aei.tuke.sk/papers/2020/2/03_Gombos.pdf)

## Repository structure

* The `data` directory contains the data which is used for the testing of the index implementation.
* The `documentation` directory contains some of the documentation of the implementation written in _Markdown_.
* The `script` directory contains _Bash_ scripts to automate the installation and testing processes.
* The `source` directory contains the source code of the index implementation written in _C_ and _SQL_.

## How to use

The indexed must be installed before it can be used. The index was developed on Linux and because of that the installtion process is designed for Linux as well.

### Install the extension

First of all, get the source code:

```shell
$ git clone https://github.com/sajtizsolt/mtree_gist
$ cd mtree_gist
```

Since this extension is not yet part of the official PostgreSQL source code, it should be installed manually. The installation can be automated with the help of the `script/install.sh` _Bash_ script.

All of the _Bash_ scripts in this repository are working with files, mostly created by _PostgreSQL_, therefore the scripts need the correct absolute file paths to the files and directories. The scripts will try to find these paths with the help of the `script/mtree_gist.properties` file, __WHICH IS NOT INCLUDED IN THIS REPOSITORY__. It should be created and filled with the correct paths.

The `script/mtree_gist.properties` file should contain the following properties:

```properties
# mtree_gist
mtree.source=/path/of/mtree_gist/root/directory

# PostgreSQL
postgresql.data=/path/of/data/directory/initialized/by/postgresql
postgresql.extension=/path/of/postgresql/extension/directory
postgresql.include=/path/of/postgresql/include/directory
postgresql.lib=/path/of/postgresql/library/directory
postgresql.server.log=/path/of/postgresql/server.log
postgresql.test=/path/of/test/data/directory/owned/by/postgres/user
```

After the file is created, run the following command:

```shell
$ sudo bash script/install.sh
```

### Try the extension

After the extension is installed successfully it is possible to use it. This is the right time to change to `postgres` user and start the server. If the server was running before the installation, it should be restarted now. Run the following commands:

```shell
$ sudo -iu postgres
$ sudo script/stop.sh
$ sudo script/start.sh
$ psql
```

After the interactive _PostgreSQL_ terminal is running, type the following _SQL_ commands:

```sql
$ DROP EXTENSION IF EXISTS mtree_gist CASCADE;
$ CREATE EXTENSION mtree_gist;
```

Now the extension is created and can be recognized by _PostgreSQL_, so it can be used.

## Development

Read [this](documentation/DEVELOPMENT.md) document for some development notes.

Read [this](documentation/TODO.md) document for the to-do list.
