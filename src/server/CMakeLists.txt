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
    schema_lib
    spdlog
    libpg_query_lib
    semantics_lib
    query_lib
    insert_lib
    ssl
    crypto
    fuck_grpc_server
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
)

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    hello.proto
)

#
# Add Library target with protobuf sources
#
add_library(myproto ${PROTO_FILES})
target_link_libraries(myproto
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
)
target_include_directories(myproto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in myproto target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
# get_target_property(grpc_cpp_plugin_location grpc::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET myproto LANGUAGE cpp)
protobuf_generate(TARGET myproto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")


#
# Dependencies
#
find_package(Threads)

#
# Sources
#
set(SOURCES
    grpc_server.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


#
# Target
#
add_library(fuck_grpc_server ${SOURCES})
target_link_libraries(fuck_grpc_server
    PRIVATE
    myproto
    spdlog
)
