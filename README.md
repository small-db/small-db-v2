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

## (book) Building a Geo-Distributed Database: A Step-by-Step Guide

/usr/lib/llvm-20/bin/clang -o bazel-out/k8-dbg/bin/test/integration-test/sql_test bazel-out/k8-dbg/bin/test/integration-test/_objs/sql_test/sql_test.o bazel-out/k8-dbg/bin/postgres/lib/libpq.a bazel-out/k8-dbg/bin/src/server/libserver.a bazel-out/k8-dbg/bin/libpg_query/lib/libpg_query.a bazel-out/k8-dbg/bin/src/query/libquery.a bazel-out/k8-dbg/bin/src/store/libstore.a bazel-out/k8-dbg/bin/arrow/lib/libarrow_substrait.a bazel-out/k8-dbg/bin/arrow/lib/libarrow_acero.a bazel-out/k8-dbg/bin/arrow/lib/libarrow_dataset.a bazel-out/k8-dbg/bin/arrow/lib/libparquet.a bazel-out/k8-dbg/bin/arrow/lib/libarrow.a bazel-out/k8-dbg/bin/arrow/lib/libarrow_bundled_dependencies.a bazel-out/k8-dbg/bin/spdlog/lib/libspdlogd.a bazel-out/k8-dbg/bin/libpqxx/lib/libpqxx.a bazel-out/k8-dbg/bin/postgres/lib/libpq.a bazel-out/k8-dbg/bin/postgres/lib/libpgcommon.a bazel-out/k8-dbg/bin/postgres/lib/libpgport.a bazel-out/k8-dbg/bin/postgres/lib/libpgport_shlib.a bazel-out/k8-dbg/bin/external/googletest+/libgtest_main.a bazel-out/k8-dbg/bin/external/googletest+/libgtest.a -pthread -pthread -lstdc++ -lm
