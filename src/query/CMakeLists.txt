# ============================================================================== #
# library: query_lib
# ============================================================================== #
add_library(query_lib
    query.cc
    query.h
)

target_link_libraries(query_lib
    PRIVATE
    arrow
    spdlog
)
