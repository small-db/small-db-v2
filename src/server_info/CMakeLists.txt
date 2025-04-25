add_library(small_server_info
    server_info.h
    server_info.cc
)

target_link_libraries(small_server_info
    PUBLIC
    absl::statusor
    PRIVATE
    spdlog::spdlog
    uuid
)

add_library(small::server_info ALIAS small_server_info)
