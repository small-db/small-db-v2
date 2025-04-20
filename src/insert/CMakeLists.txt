add_library(insert_lib
    insert.cc
    insert.h
)

target_link_libraries(insert_lib
    PUBLIC
    spdlog
    absl_status
    libpg_query_lib
    arrow_lib
    small::rocks
    small::schema
    server_registry
    magic_enum
    small::semantics
    small::encode
    insert_proto
)

# ======================================================================== #
# protobuf target
# ======================================================================== #

add_library(insert_proto
    insert.proto
)
target_link_libraries(insert_proto
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
)
target_include_directories(insert_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET insert_proto LANGUAGE cpp)
protobuf_generate(TARGET insert_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
