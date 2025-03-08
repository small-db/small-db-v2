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

# https://discourse.cmake.org/t/cmake-3-28-cmake-cxx-compiler-clang-scan-deps-notfound-not-found/9244/4
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

# # ============================================================================== #
# # external dependencies: arrow
# # ============================================================================== #
# cmake_minimum_required(VERSION 3.14)

# project(example LANGUAGES CXX)

# set(CMAKE_BUILD_TYPE Debug)


# ============================================================================== #
# third-party dependencies
# ============================================================================== #

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/third_party/")

find_package(Arrow REQUIRED)

find_package(Spdlog REQUIRED)

find_package(PgQuery REQUIRED)

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

FetchContent_MakeAvailable(libpqxx)

FetchContent_MakeAvailable(googletest)

# ============================================================================== #
# subdirectories
# ============================================================================== #

include_directories(
    ${CMAKE_SOURCE_DIR}
)

add_subdirectory(src)

# ============================================================================== #
# clutter
# ============================================================================== #

enable_testing()

add_executable(
    sql_test
    test/integration_test/sql_test.cc
)
target_link_libraries(
    sql_test
    GTest::gtest_main
    pqxx
    spdlog
    server_lib
)

include(GoogleTest)
gtest_discover_tests(sql_test)
