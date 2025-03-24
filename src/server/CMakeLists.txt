add_library(server_lib
    server.cc
    server.h
    args.cc
    args.h
)

target_link_libraries(server_lib
    PUBLIC
    query_lib
    store_lib
    spdlog
    libpg_query_lib
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
)
