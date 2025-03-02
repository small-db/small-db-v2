# small-db-v2

## Usage

### Start Cluster with Docker

TODO

## Development - General

### Style Guide

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Development - CMake

### Build System

- [CMake 3.21.3](https://cmake.org/)

## Development - Bazel

### Build System

- [bazel 8.0.1](https://bazel.build/)

### Start Server

```shell
# start server with default port 5432
bazel run //src/server:main

# start server with custom port
bazel run //src/server:main -- --port=5432
```

### Run Integration Test

```shell
bazel test //:integration_test
```

### Format Code & Run Linter

```shell
./scripts/format/run-format.sh
```
