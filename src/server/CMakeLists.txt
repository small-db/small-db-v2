# Define the library target "server_lib"
add_library(server_lib
    server.cc
    server.h
    # Add other source files and headers here
)

target_link_libraries(server_lib
    PUBLIC
    query_lib
    store_lib
    spdlog
    libpg_query_lib
)

# # Define the executable target "server"
add_executable(server
    main.cc
    # Add other source files here if needed
)

# # Link the "server_lib" library to the "server" executable
target_link_libraries(server
    PRIVATE
    server_lib
)

# # Optionally, you can set properties for the targets, such as include directories
# target_include_directories(server_lib
#     PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

# target_include_directories(server
#     PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

# # add_dependencies(server_lib libpg_query)

# # target_include_directories(server_lib PRIVATE
# #     ${CMAKE_BINARY_DIR}/install/usr/local/include # Adjust if necessary
# # )

# # target_link_directories(server_lib PRIVATE
# #     ${CMAKE_BINARY_DIR}/install/usr/local/lib  # Adjust based on install location
# # )

# # target_link_libraries(server PRIVATE libpg_query)  # Replace 'mylib' with actual library name