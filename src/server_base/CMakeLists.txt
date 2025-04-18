add_library(server_base_lib
    args.cc
    args.h
)

target_link_libraries(server_base_lib
    PRIVATE
    absl::statusor
    spdlog::spdlog
)
