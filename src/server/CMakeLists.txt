# Define the library target "server_lib"
add_library(server_lib
    server.cc
    server.h
    # Add other source files and headers here
)

# Define the executable target "server"
add_executable(server
    main.cc
    # Add other source files here if needed
)

# Link the "server_lib" library to the "server" executable
target_link_libraries(server
    PRIVATE
    server_lib
)

# Optionally, you can set properties for the targets, such as include directories
target_include_directories(server_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(server
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)