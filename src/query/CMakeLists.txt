add_library(query_lib
    query.cc
    query.h
)

target_link_libraries(query_lib
    PUBLIC
    spdlog
    absl_status
    libpg_query_lib
    arrow_lib
)
