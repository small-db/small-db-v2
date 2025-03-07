FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1
)

list(APPEND Spdlog_INCLUDE_DIRS ${spdlog_SOURCE_DIR}/include)
