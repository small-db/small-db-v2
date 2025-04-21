cmake_minimum_required(VERSION 3.28)
project(play)

find_package(absl CONFIG REQUIRED)

add_executable(server
    main.cc
)

target_link_libraries(server
    PRIVATE
    absl::status
)
