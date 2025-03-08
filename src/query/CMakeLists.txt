# ============================================================================== #
# library: query_lib
# ============================================================================== #

message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")
# message(STATUS "arrow_SOURCE_DIR (2): ${arrow_SOURCE_DIR}")

add_library(query_lib
    query.cc
    query.h
)

# find_package(Arrow REQUIRED)

# target_link_libraries(query_lib
#     PRIVATE
#     Arrow::arrow_shared
#     spdlog
# )

# add_executable(example example.cc)

# target_link_libraries(query_lib
#   PRIVATE
#   arrow_shared
#   arrow_dataset_shared
#   arrow_acero_shared
#   parquet_shared)

# message(STATUS "Arrow_LIBRARIES (2): ${Arrow_LIBRARIES}")

target_link_libraries(query_lib
    PRIVATE
    # ArrowKing
    # Arrow
    arrow
    spdlog
    # SpdlogKing
)

# add_dependencies(server_lib libpg_query)

get_property(TARGETS DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
message(STATUS "Targets: ${TARGETS}")

# target_include_directories(query_lib
#     PRIVATE
#     ${Arrow_INCLUDE_DIRS}
#     ${Spdlog_INCLUDE_DIRS}
# )

# message(STATUS "Arrow_INCLUDE_DIRS: ${Arrow_INCLUDE_DIRS}")

# message(STATUS "arrow_BINARY_DIR: ${arrow_BINARY_DIR}")
# message(STATUS "arrow_SOURCE_DIR (2): ${arrow_SOURCE_DIR}")

# target_include_directories(query_lib
#     ${arrow_BINARY_DIR}/src
#     ${arrow_SOURCE_DIR}/cpp/src)
