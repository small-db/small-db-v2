# ref:
# - https://github.com/heavyai/heavydb/blob/a5dc49c757739d87f12baf8038ccfe4d1ece88ea/cmake/Modules/FindArrow.cmake
# - https://github.com/amoeba/arrow-cmake-fetchcontent/blob/main/CMakeLists.txt

set(ARROW_BUILD_SHARED True)
set(ARROW_DEPENDENCY_SOURCE "BUNDLED")
set(ARROW_SIMD_LEVEL NONE)

set(ARROW_ACERO ON)
set(ARROW_PARQUET ON)
set(ARROW_IPC ON)
set(ARROW_DATASET ON)
set(ARROW_FILESYSTEM ON)
set(ARROW_COMPUTE ON)

set(ARROW_BUILD_TESTS OFF)

include(FetchContent)

FetchContent_Declare(Arrow
  GIT_REPOSITORY https://github.com/apache/arrow.git
  GIT_TAG apache-arrow-19.0.1
  GIT_SHALLOW TRUE
  SOURCE_SUBDIR cpp
  OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(Arrow)

list(APPEND Arrow_INCLUDE_DIRS ${arrow_SOURCE_DIR}/cpp/src)
list(APPEND Arrow_INCLUDE_DIRS ${arrow_BINARY_DIR}/src)

add_library(arrow_lib INTERFACE IMPORTED)

target_link_libraries(
  arrow_lib
  INTERFACE
  arrow
  arrow_dataset
  arrow_acero
  parquet
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

