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
    absl::status
    # grpc_server
)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    server_lib
)

add_library(grpc_server
    grpc_server.cc
    grpc_server.h
)

target_link_libraries(grpc_server
    PRIVATE
    myproto
    spdlog
    absl::status
)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads)

#
# Protobuf/Grpc source files
#
set(PROTO_FILES
    server_reg.proto
    # myproto/address.proto
    # myproto/addressbook.proto
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
protobuf_generate(TARGET myproto LANGUAGE cpp)
protobuf_generate(TARGET myproto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
