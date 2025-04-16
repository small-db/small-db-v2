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
# third-party packages
# ======================================================================== #

# step 1: update CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/third_party/")

# step 2: include FetchContent module
include(FetchContent)

# step 3: include third-party packages
find_package(Arrow REQUIRED)
find_package(Spdlog REQUIRED)
find_package(PgQuery REQUIRED)
find_package(GoogleTest REQUIRED)
# find_package(Abseil REQUIRED)
# find_package(RocksDB REQUIRED)
# find_package(Json REQUIRED)
# find_package(MagicEnum REQUIRED)
find_package(Protobuf REQUIRED)
# # find_package(Grpc REQUIRED)

# # find_package(absl CONFIG REQUIRED)

# # ============================================================================== #
# # source code
# # ============================================================================== #

# include_directories(
#     ${CMAKE_SOURCE_DIR}
# )

# # define macros
# add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

# add_subdirectory(src)
# add_subdirectory(test)

# add_subdirectory(src/fuck_cpp/grpc)
