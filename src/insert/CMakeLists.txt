add_library(insert_lib
    insert.cc
    insert.h
)

target_link_libraries(insert_lib
    PUBLIC
    spdlog
    absl_status
    libpg_query_lib
    arrow_lib
    rocksdb_wrapper_lib
    schema_lib
    server_reg_lib
    magic_enum
)
