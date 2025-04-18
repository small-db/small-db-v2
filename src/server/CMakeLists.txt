add_library(server_arg_lib
    args.cc
    args.h
)

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
    semantics_lib
    query_lib
    insert_lib
    ssl
    crypto
    server_arg_lib
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
)
