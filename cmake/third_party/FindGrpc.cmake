FetchContent_Declare(
    grpc
    GIT_REPOSITORY https://github.com/grpc/grpc.git
    GIT_TAG v1.71.0
    GIT_SHALLOW TRUE
)

get_all_targets(. BEFORE_TARGETS)

FetchContent_MakeAvailable(grpc)

get_all_targets(. AFTER_TARGETS)
print_added_target(BEFORE_TARGETS AFTER_TARGETS)
