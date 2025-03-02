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
# build settings
# ============================================================================== #
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

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

FetchContent_MakeAvailable(googletest libpqxx)

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
