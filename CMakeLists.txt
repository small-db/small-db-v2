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

# # https://discourse.cmake.org/t/cmake-3-28-cmake-cxx-compiler-clang-scan-deps-notfound-not-found/9244/4
# set(CMAKE_CXX_SCAN_FOR_MODULES 0)

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

# define macros
add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

add_subdirectory(src)
add_subdirectory(test)

message(STATUS "== CMake configuration complete ==")

# get_property(all_targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)

# foreach(tgt ${all_targets})
#     if(tgt MATCHES "::")
#         message(STATUS "Namespaced target: ${tgt}")
#     endif()
# endforeach()

# get_property(all_targets GLOBAL PROPERTY TARGETS)

# foreach(tgt ${all_targets})
#     get_target_property(is_imported ${tgt} IMPORTED)
#     if(is_imported AND tgt MATCHES "::")
#         message(STATUS "Imported namespaced target: ${tgt}")
#     endif()
# endforeach()

function(print_all_targets DIR)
  get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)
  foreach(TGT IN LISTS TGTS)
    message(STATUS "build target: ${TGT}")
  endforeach()

  get_property(TGTS DIRECTORY "${DIR}" PROPERTY ALIASED_TARGET)
  foreach(TGT IN LISTS TGTS)
    message(STATUS "aliased target: ${TGT}")
  endforeach()

  get_property(TGTS DIRECTORY "${DIR}" PROPERTY IMPORTED_TARGETS)
  foreach(TGT IN LISTS TGTS)
    message(STATUS "imported target: ${TGT}")
  endforeach()

  get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
  foreach(SUBDIR IN LISTS SUBDIRS)
    print_all_targets("${SUBDIR}")
  endforeach()
endfunction()

print_all_targets(.)
