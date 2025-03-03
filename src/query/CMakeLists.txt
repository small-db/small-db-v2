# ============================================================================== #
# library: query_lib
# ============================================================================== #
add_library(query_lib
    query.cc
    query.h
)

find_package(Arrow REQUIRED)

target_link_libraries(query_lib
    PRIVATE
    Arrow::arrow_shared
    spdlog
)
