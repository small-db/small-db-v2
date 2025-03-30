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
