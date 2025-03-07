list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/third_party)

find_package(Arrow REQUIRED)

message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")
include_directories(${Arrow_INCLUDE_DIRS})