add_library(parser_lib
    parser.cc
    parser.h
)

# if (TARGET absl::status)
#     message(WARNING "Target absl::status is NOT available.")
# endif()

# if (TARGET absl::status)
#     message(WARNING "Target absl::status is NOT available.")
# endif()

target_link_libraries(parser_lib
    PRIVATE
    spdlog
    absl::status
    absl::statusor
    absl::strings
    small::type
)
