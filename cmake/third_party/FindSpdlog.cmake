FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
)

# if (sqdlog_POPULATED)
#     message(STATUS "sqdlog_POPULATED 1: ${sqdlog_POPULATED}")
# else()
#     message(STATUS "sqdlog_POPULATED 2: ${sqdlog_POPULATED}")
# endif()

message(STATUS "spdlog_POPULATED: ${spdlog_POPULATED}")

FetchContent_MakeAvailable(spdlog)

# get_cmake_property(_vars VARIABLES)
# foreach(_var ${_vars})
#   message(STATUS "${_var}=${${_var}}")
# endforeach()

# if (sqdlog_POPULATED)
#     message(STATUS "sqdlog_POPULATED 1: ${sqdlog_POPULATED}")
# else()
#     message(STATUS "sqdlog_POPULATED 2: ${sqdlog_POPULATED}")
# endif()

message(STATUS "spdlog_POPULATED: ${spdlog_POPULATED}")

# list(APPEND Spdlog_INCLUDE_DIRS ${spdlog_SOURCE_DIR}/include)

# add_library(SpdlogKing INTERFACE IMPORTED)
# # target_link_libraries(ArrowKing INTERFACE arrow_static)
# target_include_directories(SpdlogKing
#     INTERFACE ${spdlog_SOURCE_DIR}/include)

