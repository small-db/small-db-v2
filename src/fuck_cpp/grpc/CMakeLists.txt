# include(FetchContent)
# set(FETCHCONTENT_QUIET OFF)
# FetchContent_Declare(
#     grpc
#     GIT_REPOSITORY https://github.com/grpc/grpc.git
#     GIT_TAG v1.71.0
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(grpc)

# if(TARGET grpc_cpp_plugin)
#     set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:grpc_cpp_plugin>)
#     message(STATUS "gRPC C++ plugin executable: ${_GRPC_CPP_PLUGIN_EXECUTABLE}")

#     # get_target_property(grpc_cpp_plugin_location grpc_cpp_plugin LOCATION)
#     # message(STATUS "grpc_cpp_plugin found at: ${grpc_cpp_plugin_location}")
# else()
#     message(FATAL_ERROR "grpc_cpp_plugin was not built!")
# endif()

# get_property(props TARGET grpc_cpp_plugin PROPERTY PROPERTIES)
# foreach(prop ${props})
#     get_target_property(val grpc_cpp_plugin ${prop})
#     message(STATUS "${prop} = ${val}")
# endforeach()

# if(TARGET gRPC::grpc_cpp_plugin)
#     get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
#     message(STATUS "grpc_cpp_plugin found at: ${grpc_cpp_plugin_location}")
# else()
#     message(FATAL_ERROR "grpc_cpp_plugin was not built!")
# endif()

# # find_package(Protobuf CONFIG REQUIRED)
# # find_package(gRPC CONFIG REQUIRED)
# find_package(Threads)

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
    server.cc
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


#
# Target
#
add_executable(fuck_grpc_server ${SOURCES})
target_link_libraries(fuck_grpc_server
    PRIVATE
    myproto
)
