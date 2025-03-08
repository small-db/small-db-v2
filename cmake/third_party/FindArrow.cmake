# # Summary
# 
# This module fetches arrow from remote and builds it.
# 
# # Control Variables
#
# - Arrow_USE_STATIC_LIBS  - Set to ON to force use of static libraries.
# 
# # Set Variables
#
# The following are set after the configuration is done:
#
# - Arrow_LIBRARIES        - path to the Arrow libraries
# - Arrow_LIBRARY_DIRS     - compile time link directories
# - Arrow_INCLUDE_DIRS     - compile time include directories
#
# # Sample Usage
#
# ```cmake
# find_package(Arrow)
# if(Arrow_FOUND)
#   target_link_libraries(<YourTarget> ${Arrow_LIBRARIES})
# endif()
# ```

set(ARROW_BUILD_SHARED True)
set(ARROW_DEPENDENCY_SOURCE "BUNDLED")
set(ARROW_SIMD_LEVEL NONE)

set(ARROW_ACERO ON)
set(ARROW_PARQUET ON)
set(ARROW_IPC ON)
set(ARROW_DATASET ON)
set(ARROW_FILESYSTEM ON)
set(ARROW_COMPUTE ON)

set(ARROW_BUILD_TESTS OFF)

include(FetchContent)

FetchContent_Declare(Arrow
  GIT_REPOSITORY https://github.com/apache/arrow.git
  GIT_TAG apache-arrow-19.0.1
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(Arrow)

# get_cmake_property(_vars VARIABLES)
# foreach(_var ${_vars})
#   message(STATUS "${_var}=${${_var}}")
# endforeach()

list(APPEND Arrow_INCLUDE_DIRS ${arrow_SOURCE_DIR}/cpp/src)
list(APPEND Arrow_INCLUDE_DIRS ${arrow_BINARY_DIR}/src)

# message(STATUS "Arrow_LIBRARIES: ${Arrow_LIBRARIES}")
# message(STATUS "Arrow_LIBRARY_DIRS: ${Arrow_LIBRARY_DIRS}")
# message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")
# message(STATUS "arrow_BINARY_DIR: ${arrow_BINARY_DIR}")
# message(STATUS "arrow_SOURCE_DIR (1): ${arrow_SOURCE_DIR}")

# add_library(Arrow::arrow_static INTERFACE IMPORTED)
# target_link_libraries(Arrow::arrow_static INTERFACE arrow_static)
# target_include_directories(Arrow::arrow_static
#   INTERFACE ${arrow_BINARY_DIR}/src
#   ${arrow_SOURCE_DIR}/cpp/src)

# message(STATUS "INCLUDE_DIRECTORIES: ${INCLUDE_DIRECTORIES}")
# message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")
# message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")

# include_directories(${Arrow_INCLUDE_DIRS})

# message(STATUS "INCLUDE_DIRECTORIES: ${INCLUDE_DIRECTORIES}")
# message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")

# get_cmake_property(_vars VARIABLES)
# foreach(_var ${_vars})
#     message(STATUS "${_var}=${${_var}}")
# endforeach()

if(TARGET Arrow::arrow_shared)
  message(STATUS "Arrow::arrow_shared exists")
else()
  message(STATUS "Arrow::arrow_shared does not exist")
endif()

if(TARGET arrow_shared)
  message(STATUS "Arrow::arrow_shared exists")
else()
  message(STATUS "Arrow::arrow_shared does not exist")
endif()

add_library(arrow INTERFACE IMPORTED)
# target_link_libraries(arrow INTERFACE arrow_static)
target_link_libraries(
  arrow
  INTERFACE
  arrow
  arrow_dataset
  arrow_acero
  parquet
)

target_include_directories(arrow
  INTERFACE ${arrow_BINARY_DIR}/src
  ${arrow_SOURCE_DIR}/cpp/src)
target_link_directories(
  arrow
  INTERFACE
  ${arrow_BINARY_DIR}/debug
)


get_property(TARGETS DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(TGT ${TARGETS})
  get_target_property(TYPE ${TGT} TYPE)
  if(TYPE STREQUAL "STATIC_LIBRARY" OR TYPE STREQUAL "SHARED_LIBRARY" OR TYPE STREQUAL "MODULE_LIBRARY")
    message(STATUS "Library target: ${TGT}")
  endif()
  message(STATUS "Library target: ${TGT}")
endforeach()
