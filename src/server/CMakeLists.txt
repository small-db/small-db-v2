add_library(server_lib
    server.cc
    server.h
    args.cc
    args.h
    stmt_handler.cc
    stmt_handler.h
)

target_link_libraries(server_lib
    PUBLIC
    base_lib
    schema_lib
    spdlog
    libpg_query_lib
    semantics_lib
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
)
