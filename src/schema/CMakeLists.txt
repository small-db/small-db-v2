add_library(small_schema
    const.h
    schema.cc
    schema.h
    partition.cc
    partition.h
)

target_link_libraries(small_schema
    PUBLIC
    rocksdb
    spdlog
    absl_status
    libpg_query_lib
    nlohmann_json::nlohmann_json
    rocksdb_wrapper_lib
    id_lib
    small::type
    small::encode
    server_base_lib
    insert_lib
)

add_library(small::schema ALIAS small_schema)