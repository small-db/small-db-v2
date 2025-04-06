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
    server_lib
)

include(GoogleTest)
gtest_discover_tests(sql_test)

# add_test(
#     NAME sql_test
#     COMMAND sql_test
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# )
