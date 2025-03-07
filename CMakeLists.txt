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

# add_subdirectory("third_party")
message(STATUS "arrow_SOURCE_DIR (3): ${arrow_SOURCE_DIR}")

# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     GIT_REPOSITORY https://github.com/google/googletest.git
#     GIT_TAG v1.16.0
# )

# FetchContent_Declare(
#     libpqxx
#     GIT_REPOSITORY https://github.com/jtv/libpqxx.git
#     GIT_TAG 7.7.4
# )

# FetchContent_Declare(
#     spdlog 
#     GIT_REPOSITORY https://github.com/gabime/spdlog.git
#     GIT_TAG v1.15.1
# )

# # FetchContent_Declare(
# #     arrow
# #     GIT_REPOSITORY https://github.com/apache/arrow.git
# #     GIT_TAG apache-arrow-19.0.1
# # )

# FetchContent_MakeAvailable(googletest libpqxx spdlog)

# # Add Arrow include and library directories
# FetchContent_GetProperties(arrow)
# if(NOT arrow_POPULATED)
#   FetchContent_Populate(arrow)
#   add_subdirectory(${arrow_SOURCE_DIR} ${arrow_BINARY_DIR} EXCLUDE_FROM_ALL)
# endif()

# # Set Arrow_DIR to the build directory of Arrow
# set(Arrow_DIR ${arrow_BINARY_DIR}/arrow/cmake_modules)

# find_package(Arrow REQUIRED)

# include(ExternalProject)

# # https://stackoverflow.com/questions/63667653/how-to-build-an-external-library-downloaded-with-cmake-fetchcontent

# ExternalProject_Add(
#     libpg_query
#     GIT_REPOSITORY https://github.com/small-db/libpg_query.git
#     GIT_TAG 7018e9e
#     CONFIGURE_COMMAND ""
#     BUILD_COMMAND make libpg_query.a libpg_query.so
#     INSTALL_COMMAND make install PREFIX=${CMAKE_BINARY_DIR}/install
#     BUILD_IN_SOURCE 1
# )

# ============================================================================== #
# subdirectories
# ============================================================================== #

add_subdirectory(src)

# # ============================================================================== #
# # clutter
# # ============================================================================== #

# enable_testing()

# add_executable(
#     hello_test
#     test/integration_test/hello_test.cc
# )
# target_link_libraries(
#     hello_test
#     GTest::gtest_main
#     pqxx
# )

# include(GoogleTest)
# gtest_discover_tests(hello_test)
