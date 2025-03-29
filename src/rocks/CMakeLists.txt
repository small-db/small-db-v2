# ============================================================================== #
# library: schema_lib
# ============================================================================== #

add_library(rocks_wrapper_lib
    wrapper.cc
    wrapper.h
)

target_link_libraries(rocks_wrapper_lib
    PUBLIC
    spdlog
    absl_status
    rocksdb
)
