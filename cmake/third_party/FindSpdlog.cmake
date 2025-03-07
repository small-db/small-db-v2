FetchContent_Declare(
    spdlog 
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
)

message(STATUS "spdlog_BINARY_DIR: ${spdlog_BINARY_DIR}")
message(STATUS "spdlog_SOURCE_DIR: ${spdlog_SOURCE_DIR}")

# list(APPEND Spdlog_INCLUDE_DIRS ${spdlog_SOURCE_DIR}/cpp/src)
list(APPEND Spdlog_INCLUDE_DIRS ${spdlog_SOURCE_DIR}/include)