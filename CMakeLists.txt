# ============================================================================== #
# base configuration
# ============================================================================== #
cmake_minimum_required(VERSION 3.28)
project(small-db-v2)

# ============================================================================== #
# compiler settings
# ============================================================================== #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================== #
# external dependencies: arrow
# ============================================================================== #
cmake_minimum_required(VERSION 3.14)

project(example LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

set(ARROW_BUILD_SHARED True)
set(ARROW_DEPENDENCY_SOURCE "SYSTEM")
set(ARROW_SIMD_LEVEL NONE) # macOS-specific workaround

# Turn on Arrow features here (and add them to target_link_libraries when
# appropriate)
set(ARROW_ACERO ON)
set(ARROW_PARQUET ON)
set(ARROW_IPC ON)
set(ARROW_DATASET ON)
set(ARROW_FILESYSTEM ON)
set(ARROW_COMPUTE ON)

# Get Arrow
include(FetchContent)

FetchContent_Declare(Arrow
  GIT_REPOSITORY https://github.com/apache/arrow.git
  GIT_TAG main
  GIT_SHALLOW TRUE SOURCE_SUBDIR cpp
  OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(Arrow)

add_executable(example example.cc)
target_link_libraries(example
  PRIVATE
  arrow_shared
  arrow_dataset_shared
  arrow_acero_shared
  parquet_shared)

# ============================================================================== #
# external dependencies
# ============================================================================== #
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0
)

FetchContent_Declare(
    libpqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.7.4
)

FetchContent_Declare(
    spdlog 
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
)

# FetchContent_Declare(
#     arrow
#     GIT_REPOSITORY https://github.com/apache/arrow.git
#     GIT_TAG apache-arrow-19.0.1
# )

FetchContent_MakeAvailable(googletest libpqxx spdlog)

# Add Arrow include and library directories
FetchContent_GetProperties(arrow)
if(NOT arrow_POPULATED)
  FetchContent_Populate(arrow)
  add_subdirectory(${arrow_SOURCE_DIR} ${arrow_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# Set Arrow_DIR to the build directory of Arrow
set(Arrow_DIR ${arrow_BINARY_DIR}/arrow/cmake_modules)

find_package(Arrow REQUIRED)

include(ExternalProject)

# https://stackoverflow.com/questions/63667653/how-to-build-an-external-library-downloaded-with-cmake-fetchcontent

ExternalProject_Add(
    libpg_query
    GIT_REPOSITORY https://github.com/small-db/libpg_query.git
    GIT_TAG 7018e9e
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make libpg_query.a libpg_query.so
    INSTALL_COMMAND make install PREFIX=${CMAKE_BINARY_DIR}/install
    BUILD_IN_SOURCE 1
)

# ============================================================================== #
# subdirectories
# ============================================================================== #

add_subdirectory(src)

# ============================================================================== #
# clutter
# ============================================================================== #

enable_testing()

add_executable(
    hello_test
    test/integration_test/hello_test.cc
)
target_link_libraries(
    hello_test
    GTest::gtest_main
    pqxx
)

include(GoogleTest)
gtest_discover_tests(hello_test)
