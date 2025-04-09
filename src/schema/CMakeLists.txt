add_library(schema_lib
    const.h
    schema.cc
    schema.h
    partition.cc
    partition.h
)

target_link_libraries(schema_lib
    PUBLIC
    rocksdb
    spdlog
    absl_status
    libpg_query_lib
    nlohmann_json::nlohmann_json
    rocksdb_wrapper_lib
    id_lib
)
