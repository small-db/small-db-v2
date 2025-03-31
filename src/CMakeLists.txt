# ============================================================================== #
# subdirectories
# ============================================================================== #

message(STATUS "arrow_SOURCE_DIR (2): ${arrow_SOURCE_DIR}")

add_subdirectory(base)
add_subdirectory(id)
add_subdirectory(rocks)
add_subdirectory(schema)
add_subdirectory(store)
add_subdirectory(semantics)
add_subdirectory(server)