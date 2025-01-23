#include <array>
#include <cstdio>
#include <cstdlib>
#include <gtest/gtest.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>


// Test fixture for setting up and tearing down the server
class SQLTest : public ::testing::Test {
    protected:
    void SetUp() override {
        SPDLOG_INFO("starting the server");
        StartServer();
    }

    void StartServer() {
        pid_t c_pid = fork();
        if (c_pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (c_pid > 0) {
            // Parent process
        } else {
            // Child process
        }
    }

    void TearDown() override {
        SPDLOG_INFO("stopping the server");
    }
};

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) {
    SPDLOG_INFO("run test: ExecuteSimpleSQL");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
