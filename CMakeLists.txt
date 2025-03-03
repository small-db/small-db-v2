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

FetchContent_Declare(
    spdlog 
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
)

FetchContent_Declare(
    arrow
    GIT_REPOSITORY https://github.com/apache/arrow.git
    GIT_TAG apache-arrow-19.0.1
)

FetchContent_MakeAvailable(googletest libpqxx spdlog arrow)

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
