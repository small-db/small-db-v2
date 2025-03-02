#include <gtest/gtest.h>

#include <pqxx/pqxx>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
    // Expect two strings not to be equal.
    EXPECT_STRNE("hello", "world");
    // Expect equality.
    EXPECT_EQ(7 * 6, 42);

    pqxx::connection conn = pqxx::connection{
        "dbname=postgres user=postgres password=postgres "
        "hostaddr=127.0.0.1 port=5432"};
}