#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory>
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
            SPDLOG_INFO("child process");
            exit(EXIT_SUCCESS);
        }
    }

    void TearDown() override { SPDLOG_INFO("stopping the server"); }
};

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) { SPDLOG_INFO("run test: ExecuteSimpleSQL"); }
