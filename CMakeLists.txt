# ============================================================================== #
# base configuration
# ============================================================================== #
cmake_minimum_required(VERSION 3.28)
project(my_project)

# ============================================================================== #
# compiler settings
# ============================================================================== #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# FetchContent_Declare(
#     libpq_query
#     GIT_REPOSITORY https://github.com/pganalyze/libpg_query.git
#     GIT_TAG 17-6.0.0
# )

FetchContent_MakeAvailable(googletest libpqxx)

include(ExternalProject)

# https://stackoverflow.com/questions/63667653/how-to-build-an-external-library-downloaded-with-cmake-fetchcontent

ExternalProject_Add(
    libpg_query
    GIT_REPOSITORY https://github.com/pganalyze/libpg_query.git
    GIT_TAG 17-6.0.0
    CONFIGURE_COMMAND ""
    BUILD_COMMAND make
    INSTALL_COMMAND ""
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
