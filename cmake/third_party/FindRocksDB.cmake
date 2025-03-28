set(WITH_GFLAGS OFF)
set(ROCKSDB_BUILD_SHARED OFF)
set(WITH_TESTS OFF)
set(WITH_BENCHMARK_TOOLS OFF)

FetchContent_Declare(RocksDB
  GIT_REPOSITORY https://github.com/facebook/rocksdb.git
  GIT_TAG v9.10.0
  GIT_SHALLOW TRUE
)

# get_all_targets(. BEFORE_TARGETS)

FetchContent_MakeAvailable(RocksDB)

# get_all_targets(. AFTER_TARGETS)
# print_added_target(BEFORE_TARGETS AFTER_TARGETS)