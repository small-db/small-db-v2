add_library(small_server
    server.cc
    server.h
    stmt_handler.cc
    stmt_handler.h
)

target_link_libraries(small_server
    PUBLIC
    small::server_info
    PRIVATE
    small::schema
    spdlog::spdlog
    libpg_query_lib
    small::semantics
    query_lib
    insert_lib
    ssl
    crypto
    small::util::ip
    server_registry_proto
    insert_proto
    small::catalog
    absl::status
    small::gossip
)

add_library(small::server ALIAS small_server)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    small::server
    spdlog::spdlog
    CLI11::CLI11
)
