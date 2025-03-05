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

message(STATUS "arrow_BINARY_DIR: ${arrow_BINARY_DIR}")
message(STATUS "arrow_SOURCE_DIR: ${arrow_SOURCE_DIR}")

add_library(Arrow::arrow_static INTERFACE IMPORTED)
target_link_libraries(Arrow::arrow_static INTERFACE arrow_static)
target_include_directories(Arrow::arrow_static
  INTERFACE ${arrow_BINARY_DIR}/src
  ${arrow_SOURCE_DIR}/cpp/src)

# message(STATUS "INCLUDE_DIRECTORIES: ${INCLUDE_DIRECTORIES}")
# message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")
# message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# include_directories(${Arrow_INCLUDE_DIRS})

# message(STATUS "INCLUDE_DIRECTORIES: ${INCLUDE_DIRECTORIES}")
# message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")