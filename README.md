# small-db-v2

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
cmake -S . -B build --preset=debug
cmake --build build
```

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
