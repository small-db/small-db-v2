# ============================================================================== #
# base configuration
# ============================================================================== #
cmake_minimum_required(VERSION 3.28)
project(small-db-v2)

# ============================================================================== #
# compiler settings
# ============================================================================== #

set(CMAKE_CXX_STANDARD 20)

# ======================================================================== #
# helper functions
# ======================================================================== #

add_subdirectory(cmake/helper)

# ======================================================================== #
# third-party packages (system wide)
# ======================================================================== #

# "gRPC" is installed on system wide since it's too large.
# 
# "Protobuf" and "absl" are bundled with gRPC.
# 
# If we include them locally (e.g. via FetchContent) as well, it will violate the
# "one definition rule" (ODR) and cause linking errors. See:
# 
# https://github.com/abseil/abseil-cpp/blob/master/FAQ.md#what-is-abi-and-why-dont-you-recommend-using-a-pre-compiled-version-of-abseil

find_package(Protobuf CONFIG REQUIRED)

# "gRPC" must comes after "Protobuf" since it depends on it.
find_package(gRPC CONFIG REQUIRED)

find_package(absl CONFIG REQUIRED)

# ======================================================================== #
# third-party packages (local)
# ======================================================================== #

# step 1: update CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/third_party/")

# step 2: include FetchContent module
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# step 3: include third-party packages
find_package(Arrow REQUIRED)
find_package(Spdlog REQUIRED)
find_package(PgQuery REQUIRED)
find_package(GoogleTest REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Json REQUIRED)
find_package(MagicEnum REQUIRED)

# ============================================================================== #
# source code
# ============================================================================== #

include_directories(
    ${CMAKE_SOURCE_DIR}
)

# define macros
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

add_subdirectory(src)
add_subdirectory(test)

# add_subdirectory(src/fuck_cpp/grpc)