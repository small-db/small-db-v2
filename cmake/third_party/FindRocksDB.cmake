block()
  set(WITH_GFLAGS OFF CACHE BOOL "gflags")
  set(ROCKSDB_BUILD_SHARED OFF CACHE BOOL "rocksdb build shared")
  set(WITH_TESTS OFF CACHE BOOL "rocksdb without tests")
  set(WITH_BENCHMARK_TOOLS OFF CACHE BOOL "rocksdb without benchmarking")

  FetchContent_Declare(RocksDB
    GIT_REPOSITORY https://github.com/facebook/rocksdb.git
    GIT_TAG v9.10.0
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(RocksDB)
endblock()
