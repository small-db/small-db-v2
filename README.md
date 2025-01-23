# small-db-v2

## Usage

### Start Cluster with Docker

TODO

## Development

### Build System

- [bazel 7.4.1](https://bazel.build/)

### Style Guide

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

### Start Server

```shell
bazel run //src/server:main -- 5432
```

### Run Integration Test

```shell
bazel test //:integration_test
```
