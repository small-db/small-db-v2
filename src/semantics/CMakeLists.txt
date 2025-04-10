add_library(semantics_lib
    check.cc
    check.h
)

target_link_libraries(semantics_lib
    PUBLIC
    base_lib
    absl_base
    libpg_query_lib
    magic_enum
    spdlog
)
