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
# configure with "debug" preset
cmake -S . -B build --preset=debug
# build all targets
cmake --build build
```

### Start Server

```shell
# start server with custom port
./build/src/server/server --port=5432
```

### Run Integration Test

```shell
ctest --test-dir build
```

### Format Code & Run Linter

```shell
# TODO: this is broken
./scripts/format/run-format.sh
```
