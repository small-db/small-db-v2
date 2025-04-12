# small-db-v2

[![build](https://github.com/small-db/small-db-v2/actions/workflows/ci.yml/badge.svg)](https://github.com/small-db/small-db-v2/actions/workflows/ci.yml)

## Usage

### Start Cluster with Docker

TODO

## Development

### Style Guide

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

### Build System

- [CMake 3.21.3](https://cmake.org/)

### CMake Configuration & Build

```shell
# configure with "debug" preset
cmake --preset=debug
# build all targets
cmake --build ./build/debug
```

### Start Server

```shell
# start server with custom port
./build/src/server/server --port=5432
```

### Run Integration Test

```shell
ctest --test-dir build
./build/sql_test
```

### Format Code & Run Linter

```shell
# TODO: this is broken
./scripts/format/run-format.sh
```

## Partitioning

- https://www.cockroachlabs.com/docs/stable/partitioning
- https://www.postgresql.org/docs/current/ddl-partitioning.html
- https://rasiksuhail.medium.com/guide-to-postgresql-table-partitioning-c0814b0fbd9b


> A database may only be opened by one process at a time. - https://github.com/facebook/rocksdb/wiki/basic-operations#concurrency