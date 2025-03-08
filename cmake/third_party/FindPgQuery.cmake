FetchContent_Declare(
    libpg_query
    GIT_REPOSITORY https://github.com/pganalyze/libpg_query.git
    GIT_TAG 17-6.0.0
)

message(STATUS "libpg_query_POPULATED: ${libpg_query_POPULATED}")

if(NOT libpg_query_POPULATED)
    FetchContent_Populate(libpg_query)

    # Build the library using make
    execute_process(COMMAND make -j libpg_query.a libpg_query.so
        WORKING_DIRECTORY ${libpg_query_SOURCE_DIR}
    )

    # # Optionally install the library
    # execute_process(COMMAND make install
    #     WORKING_DIRECTORY ${libpg_query_SOURCE_DIR}
    # )
endif()

# FetchContent_MakeAvailable(libpg_query)

message(STATUS "libpg_query_POPULATED: ${libpg_query_POPULATED}")

# get_cmake_property(_vars VARIABLES)
# foreach(_var ${_vars})
#     message(STATUS "${_var}=${${_var}}")
# endforeach()

add_library(libpg_query INTERFACE IMPORTED)
# target_link_libraries(ArrowKing INTERFACE arrow_static)
target_include_directories(
    libpg_query
    INTERFACE
    ${libpg_query_SOURCE_DIR}
    ${libpg_query_SOURCE_DIR}/protobuf
    ${libpg_query_SOURCE_DIR}/vendor
)

target_link_libraries(
    libpg_query
    INTERFACE
    ${libpg_query_SOURCE_DIR}/libpg_query.a
    ${libpg_query_SOURCE_DIR}/libpg_query.so
)
