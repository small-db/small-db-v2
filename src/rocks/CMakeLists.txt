add_library(small_rocks
    rocks.h
    rocks.cc
)

target_link_libraries(small_rocks
    PUBLIC
    rocksdb
    PRIVATE
    spdlog
    absl_status
)

add_library(small::rocks ALIAS small_rocks)
