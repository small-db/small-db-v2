#include <array>
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <stdexcept>
#include <string>

// Helper function to execute a shell command and capture the output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Test fixture for setting up and tearing down the server
class SQLTest : public ::testing::Test {
    protected:
    void SetUp() override {
        // Start the server
        server_process = popen("path/to/server --start", "r");
        ASSERT_TRUE(server_process != nullptr) << "Failed to start server";
        // Wait for the server to be ready
        // std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    void TearDown() override {
        // Stop the server
        if (server_process) {
            pclose(server_process);
        }
    }

    FILE* server_process = nullptr;
};

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) {
    // Create a table
    std::string create_table_result =
    exec("path/to/sql_client -c 'CREATE TABLE test (id INT, name TEXT);'");
    EXPECT_EQ(create_table_result, "Table created successfully\n");

    // Insert a row
    std::string insert_result = exec("path/to/sql_client -c 'INSERT INTO test "
                                     "(id, name) VALUES (1, \"Alice\");'");
    EXPECT_EQ(insert_result, "1 row inserted\n");

    // Select the row
    std::string select_result =
    exec("path/to/sql_client -c 'SELECT * FROM test;'");
    EXPECT_EQ(select_result, "1 | Alice\n");

    // Drop the table
    std::string drop_table_result =
    exec("path/to/sql_client -c 'DROP TABLE test;'");
    EXPECT_EQ(drop_table_result, "Table dropped successfully\n");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
