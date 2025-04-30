cmake_minimum_required(VERSION 3.28)
project(play)

find_package(absl CONFIG REQUIRED)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    absl::status
    lib_a
    lib_b
)

add_library(lib_a
    lib_a.h
    lib_a.cc
)

target_link_libraries(lib_a
    PRIVATE
    lib_b
)

add_library(lib_b
    lib_b.h
    lib_b.cc
)

target_link_libraries(lib_b
    PRIVATE
    lib_a
)
