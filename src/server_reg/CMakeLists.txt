add_library(server_reg_lib
    server_reg.cc
    server_reg.h
)

target_link_libraries(server_reg_lib
    PUBLIC
    spdlog
    absl::status
)
