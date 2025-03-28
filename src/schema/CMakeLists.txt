# ============================================================================== #
# library: schema_lib
# ============================================================================== #

add_library(schema_lib
    const.h
    schema.cc
    schema.h
)

target_link_libraries(schema_lib
    PUBLIC
    # arrow_lib
    Arrow
    spdlog
    absl_status
    libpg_query_lib
)
