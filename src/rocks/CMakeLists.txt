# ============================================================================== #
# library: small::schema
# ============================================================================== #

add_library(rocksdb_wrapper_lib
    wrapper.cc
    wrapper.h
)

target_link_libraries(rocksdb_wrapper_lib
    PUBLIC
    spdlog
    absl_status
    rocksdb
)
