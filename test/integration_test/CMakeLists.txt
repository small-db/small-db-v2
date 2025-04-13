enable_testing()

add_executable(
    sql_test
    sql_test.cc
)
target_link_libraries(
    sql_test
    type_lib
    GTest::gtest_main
    pqxx
    server_lib
    parser_lib
)

target_compile_definitions(sql_test PRIVATE GTEST_HAS_CXXABI_H_=0)

include(GoogleTest)
gtest_discover_tests(sql_test)