# ============================================================================== #
# library: schema_lib
# ============================================================================== #

add_library(base_lib
    base.h
)

target_link_libraries(base_lib
    PUBLIC
    spdlog
)
