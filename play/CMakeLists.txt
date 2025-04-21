cmake_minimum_required(VERSION 3.28)
project(play)

find_package(absl CONFIG REQUIRED)

add_executable(server
    main.cc
)

# if (TARGET absl::status)
#     message(WARNING "Target absl::status is available.")
# endif()

# if (TARGET absl::status)
#     message(WARNING "Target absl::status is available.")
# endif()

target_link_libraries(server
    PRIVATE
    absl::status
)
