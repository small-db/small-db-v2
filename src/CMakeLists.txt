# ============================================================================== #
# subdirectories
# ============================================================================== #

message(STATUS "arrow_SOURCE_DIR (2): ${arrow_SOURCE_DIR}")

add_subdirectory(rocks)
add_subdirectory(query)
add_subdirectory(schema)
add_subdirectory(store)
add_subdirectory(semantics)
add_subdirectory(server)