enable_testing()

add_executable(
    sql_test
    sql_test.cc
)
target_link_libraries(
    sql_test
    base_lib
    GTest::gtest_main
    pqxx
    spdlog
    server_lib
    absl_status
)

include(GoogleTest)
gtest_discover_tests(sql_test)
