add_library(server_lib
    server.cc
    server.h
    stmt_handler.cc
    stmt_handler.h
)

target_link_libraries(server_lib
    PUBLIC
    schema_lib
    spdlog
    libpg_query_lib
    small::semantics
    query_lib
    insert_lib
    ssl
    crypto
    server_base_lib
    small::util::ip
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
    CLI11::CLI11
)
