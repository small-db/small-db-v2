# ============================================================================== #
# library: query_lib
# ============================================================================== #
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

target_link_libraries(query_lib
    PRIVATE
    # Arrow::arrow_static
    arrow_static
)

message(STATUS "arrow_BINARY_DIR: ${arrow_BINARY_DIR}")
message(STATUS "arrow_SOURCE_DIR: ${arrow_SOURCE_DIR}")

target_include_directories(query_lib
    ${arrow_BINARY_DIR}/src
    ${arrow_SOURCE_DIR}/cpp/src)
