add_library(server_lib
    server.cc
    server.h
    stmt_handler.cc
    stmt_handler.h
)

target_link_libraries(server_lib
    PUBLIC
    small::schema
    spdlog
    libpg_query_lib
    small::semantics
    query_lib
    insert_lib
    ssl
    crypto
    server_base_lib
    small::util::ip
    server_registry_proto
    insert_proto
    small::catalog
    absl::status
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
    CLI11::CLI11
)
