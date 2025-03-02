// Copyright 2025 Xiaochen Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

// set level for "SPDLOG_<LEVEL>" macros
// NB: must define SPDLOG_ACTIVE_LEVEL before `#include "spdlog/spdlog.h"`
// to make it works
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include <spdlog/spdlog.h>

#include <cstdio>
#include <cstdlib>
#include <pqxx/pqxx>
#include <string>

#include "src/server/server.h"

// Test fixture for setting up and tearing down the server
class SQLTest : public ::testing::Test {
   protected:
    void SetUp() override {
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%@] %v");

        SPDLOG_INFO("starting the server");
        StartServer();
        WaitServer();
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
            int exit_code = server::RunServer(server::DefaultArgs);
            ASSERT_EQ(exit_code, 0);
            exit(exit_code);
        }
    }

    // wait for the server to ready
    void WaitServer() {
        // sleep for 5 seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));

        pqxx::connection conn = pqxx::connection{
            "dbname=postgres user=postgres password=postgres "
            "hostaddr=127.0.0.1 port=5432"};
        auto version = conn.server_version();
        SPDLOG_INFO("server version: {}", version);
    }

    void TearDown() override { SPDLOG_INFO("stopping the server"); }
};

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) {
    SPDLOG_INFO("start test: ExecuteSimpleSQL");

    // pqxx::work tx(this->cx);
    // tx.exec(
    //     "CREATE TABLE users (id INT PRIMARY KEY, name STRING, balance INT)");
    // tx.commit();

    std::this_thread::sleep_for(std::chrono::seconds(5));
    SPDLOG_INFO("stop test: ExecuteSimpleSQL");
}
