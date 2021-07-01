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
