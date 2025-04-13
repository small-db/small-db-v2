# ref:
# - https://github.com/heavyai/heavydb/blob/a5dc49c757739d87f12baf8038ccfe4d1ece88ea/cmake/Modules/FindArrow.cmake
# - https://github.com/amoeba/arrow-cmake-fetchcontent/blob/main/CMakeLists.txt

block()
  set(ARROW_BUILD_STATIC ON)
  set(ARROW_DEPENDENCY_SOURCE "BUNDLED")
  set(ARROW_SIMD_LEVEL NONE)
  set(ARROW_ACERO ON CACHE BOOL "acero")
  set(ARROW_PARQUET ON)
  set(ARROW_IPC ON)
  set(ARROW_DATASET ON)
  set(ARROW_FILESYSTEM ON)
  set(ARROW_COMPUTE ON)
  set(ARROW_BUILD_TESTS OFF)

  set(ARROW_GANDIVA ON CACHE BOOL "build the gandiva library")
  set(ARROW_DEFINE_OPTIONS ON)

  # set(ARROW_WITH_BROTLI ON CACHE BOOL "build with brotli support")
  # set(ARROW_WITH_BZ2 ON CACHE BOOL "build with bzip2 support")
  # set(ARROW_WITH_LZ4 ON CACHE BOOL "build with lz4 support")
  # set(ARROW_WITH_RE2 ON CACHE BOOL "build with re2 support")
  # set(ARROW_WITH_SNAPPY ON CACHE BOOL "build with snappy support")
  # set(ARROW_WITH_UTF8PROC ON CACHE BOOL "build with utf8proc support")
  # set(ARROW_WITH_ZLIB ON CACHE BOOL "build with zlib support")
  # set(ARROW_WITH_ZSTD ON CACHE BOOL "build with zstd support")
        # "ARROW_WITH_BROTLI": "ON",
        # "ARROW_WITH_BZ2": "ON",
        # "ARROW_WITH_LZ4": "ON",
        # "ARROW_WITH_RE2": "ON",
        # "ARROW_WITH_SNAPPY": "ON",
        # "ARROW_WITH_UTF8PROC": "ON",
        # "ARROW_WITH_ZLIB": "ON",
        # "ARROW_WITH_ZSTD": "ON"

  # due to a problem compiling on clang++ 18.1.3 we need to disable deprecated
  # declaration errors
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")

  FetchContent_Declare(Arrow
    GIT_REPOSITORY https://github.com/apache/arrow.git
    GIT_TAG apache-arrow-19.0.1
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR cpp
  )

  get_all_targets(. BEFORE_TARGETS)

  FetchContent_MakeAvailable(Arrow)

  get_all_targets(. AFTER_TARGETS)
  print_added_target(BEFORE_TARGETS AFTER_TARGETS)

  add_library(arrow_lib INTERFACE IMPORTED)

  target_link_libraries(
    arrow_lib
    INTERFACE
    arrow_static
    arrow_dataset_static
    arrow_acero_static
    parquet_static
  )

  target_include_directories(
    arrow_lib
    INTERFACE
    ${arrow_BINARY_DIR}/src
    ${arrow_SOURCE_DIR}/cpp/src
  )

  target_link_directories(
    arrow_lib
    INTERFACE
    ${arrow_BINARY_DIR}/debug
  )

endblock()
