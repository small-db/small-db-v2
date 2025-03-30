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
    arrow_lib
    rocksdb
    spdlog
    absl_status
    libpg_query_lib
    nlohmann_json::nlohmann_json
    rocksdb_wrapper_lib
    id_lib
)
