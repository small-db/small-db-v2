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
# find_package(Arrow REQUIRED)
# find_package(Spdlog REQUIRED)
# find_package(PgQuery REQUIRED)
# find_package(GoogleTest REQUIRED)
# find_package(Abseil REQUIRED)

FetchContent_Declare(
    abseil
    GIT_REPOSITORY "https://github.com/abseil/abseil-cpp.git"
    GIT_TAG "lts_2025_01_27"
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
    GIT_TAG "v30.2"
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    grpc
    GIT_REPOSITORY "https://github.com/grpc/grpc.git"
    GIT_TAG "v1.71.0"
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(abseil protobuf grpc)

# get_target_property(tmp absl::strings INTERFACE_LINK_LIBRARIES)
# message(STATUS "Abseil strings dependencies: ${tmp}")

# get_property(all_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
# message(STATUS "Targets: ${all_targets}")


# find_package(RocksDB REQUIRED)
# find_package(Json REQUIRED)
# find_package(MagicEnum REQUIRED)
# find_package(Protobuf REQUIRED)
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

add_subdirectory(src/fuck_cpp/grpc)
