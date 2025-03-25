# ============================================================================== #
# library: schema_lib
# ============================================================================== #

add_library(schema_lib
    schema.cc
    schema.h
)

target_link_libraries(schema_lib
    PUBLIC
    arrow_lib
    spdlog
)
