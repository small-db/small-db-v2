# ref:
# - https://github.com/heavyai/heavydb/blob/a5dc49c757739d87f12baf8038ccfe4d1ece88ea/cmake/Modules/FindArrow.cmake
# - https://github.com/amoeba/arrow-cmake-fetchcontent/blob/main/CMakeLists.txt

# set(ARROW_BUILD_SHARED True)
# set(ARROW_BUILD_SHARED False)
set(ARROW_BUILD_STATIC On)
set(ARROW_DEPENDENCY_SOURCE "BUNDLED")
set(ARROW_SIMD_LEVEL NONE)

set(ARROW_ACERO ON)
set(ARROW_PARQUET ON)
set(ARROW_IPC ON)
set(ARROW_DATASET ON)
set(ARROW_FILESYSTEM ON)
set(ARROW_COMPUTE ON)

set(ARROW_BUILD_TESTS OFF)


FetchContent_Declare(Arrow
  GIT_REPOSITORY https://github.com/apache/arrow.git
  GIT_TAG apache-arrow-19.0.1
  GIT_SHALLOW TRUE
  SOURCE_SUBDIR cpp
)

get_property(targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)

message(STATUS "Available targets:")
foreach(target IN LISTS targets)
  message(STATUS " - ${target}")
endforeach()

FetchContent_MakeAvailable(Arrow)

# get_property(targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
# get_property(targets GLOBAL PROPERTY BUILDSYSTEM_TARGETS)
get_property(targets GLOBAL PROPERTY IMPORTED_TARGETS)

message(STATUS "targets: ${targets}")

message(STATUS "Available targets:")
foreach(target IN LISTS targets)
  message(STATUS " - ${target}")
endforeach()

get_property(targets DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY IMPORTED_TARGETS)
message(STATUS "targets: ${targets}")

get_property(all_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
message(STATUS "all_targets: ${all_targets}")


# list(APPEND Arrow_INCLUDE_DIRS ${arrow_SOURCE_DIR}/cpp/src)
# list(APPEND Arrow_INCLUDE_DIRS ${arrow_BINARY_DIR}/src)

# add_library(arrow_lib INTERFACE IMPORTED)

# target_link_libraries(
#   arrow_lib
#   INTERFACE
#   arrow
#   arrow_dataset
#   arrow_acero
#   parquet
# )

# target_include_directories(
#   arrow_lib
#   INTERFACE
#   ${arrow_BINARY_DIR}/src
#   ${arrow_SOURCE_DIR}/cpp/src
# )

# target_link_directories(
#   arrow_lib
#   INTERFACE
#   ${arrow_BINARY_DIR}/debug
# )

