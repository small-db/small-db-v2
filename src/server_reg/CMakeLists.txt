add_library(server_reg_lib
    server_reg.cc
    server_reg.h
)

target_link_libraries(server_reg_lib
    PUBLIC
    spdlog
    absl::status
    server_reg_proto
)

# ======================================================================== #
# protobuf target
# ======================================================================== #

add_library(server_reg_proto
    server_reg.proto
)
target_link_libraries(server_reg_proto
    PUBLIC
    protobuf::libprotobuf
    gRPC::grpc
    gRPC::grpc++
)
target_include_directories(server_reg_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET server_reg_proto LANGUAGE cpp)
protobuf_generate(TARGET server_reg_proto LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")
