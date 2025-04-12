add_library(base_lib
    singleton.cc
    singleton.h
)

target_link_libraries(base_lib
    PUBLIC
    spdlog
    absl_status
)

# =====================================================================
# Test
# =====================================================================

enable_testing()

add_executable(
    singleton_test
    singleton_test.cc
)
target_link_libraries(
    singleton_test
    base_lib
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(singleton_test)
