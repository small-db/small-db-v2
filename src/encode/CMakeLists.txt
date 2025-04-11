add_library(encode_lib
    encode.cc
    encode.h
)

target_link_libraries(encode_lib
    PUBLIC
    spdlog
    absl_status
)
