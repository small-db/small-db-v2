# ============================================================================== #
# base configuration
# ============================================================================== #
cmake_minimum_required(VERSION 3.28)
project(small-db-v2)

set(CMAKE_FILEAPI_FILE "${CMAKE_BINARY_DIR}/cmake-api.json")
set(CMAKE_FILEAPI_ALLOW_RECURSIVE ON)

# ============================================================================== #
# compiler settings
# ============================================================================== #
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# https://discourse.cmake.org/t/cmake-3-28-cmake-cxx-compiler-clang-scan-deps-notfound-not-found/9244/4
set(CMAKE_CXX_SCAN_FOR_MODULES 0)

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
find_package(Spdlog REQUIRED)
find_package(PgQuery REQUIRED)
# find_package(Libpqxx REQUIRED)
find_package(GoogleTest REQUIRED)
find_package(Abseil REQUIRED)
find_package(RocksDB REQUIRED)
find_package(Json REQUIRED)
find_package(MagicEnum REQUIRED)

# ============================================================================== #
# source code
# ============================================================================== #

include_directories(
    ${CMAKE_SOURCE_DIR}
)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

add_subdirectory(src)
add_subdirectory(test)

get_property(all_targets GLOBAL PROPERTY TARGETS)

foreach(tgt ${all_targets})
  get_target_property(alias_check ${tgt} ALIASED_TARGET)
  if(alias_check)
    message(STATUS "[ALIAS] ${tgt} → ${alias_check}")
  else()
    get_target_property(type ${tgt} TYPE)
    message(STATUS "[${type}] ${tgt}")
  endif()
endforeach()

# Get all targets in the current CMake project
get_property(all_targets GLOBAL PROPERTY TARGETS)

# Iterate over all targets and print their type (including aliases and interfaces)
foreach(target ${all_targets})
    get_target_property(target_type ${target} TYPE)
    if(NOT target_type)
        message(STATUS "Target ${target} has no TYPE property")
    else()
        message(STATUS "[${target_type}] ${target}")
    endif()
endforeach()