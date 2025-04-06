add_library(semantics_lib
    check.cc
    check.h
)

target_link_libraries(semantics_lib
    PUBLIC
    absl_base
    libpg_query_lib
    magic_enum
    spdlog
)
