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

class SmallEnvironment : public ::testing::Environment {
   public:
    ~SmallEnvironment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override {
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%@] %v");

        SPDLOG_INFO("setting up the environment");
    }

    // Override this to define how to tear down the environment.
    void TearDown() override { SPDLOG_INFO("tearing down the environment"); }
};

const std::string CONNECTION_STRING =
    "dbname=postgres user=postgres password=postgres hostaddr=127.0.0.1 "
    "port=5432";

// Test fixture for setting up and tearing down the server
class SQLTest : public ::testing::Test {
   protected:
    static void SetUpTestSuite() {
        SPDLOG_INFO("starting the server");
        StartServerThread();
        WaitServer();
    }

    static std::thread server_thread;

    static void StartServerThread() {
        SPDLOG_INFO("starting the server thread");
        server::ServerArgs args = server::ServerArgs(5432);
        server_thread = std::thread(server::RunServer, args);
    }

    // wait for the server to ready
    static void WaitServer() {
        std::this_thread::sleep_for(std::chrono::seconds(2));

        pqxx::connection conn = pqxx::connection{CONNECTION_STRING};
        auto version = conn.server_version();
        SPDLOG_INFO("server version: {}", version);
    }

    static void TearDownTestSuite() {
        SPDLOG_INFO("stopping the server");
        server::StopServer();
        server_thread.join();
    }
};

std::thread SQLTest::server_thread;

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) {
    SPDLOG_INFO("start test: ExecuteSimpleSQL");

    pqxx::connection conn = pqxx::connection{CONNECTION_STRING};

    pqxx::work tx(conn);
    tx.exec(
        "CREATE TABLE users (id INT PRIMARY KEY, name STRING, balance INT)");
    tx.commit();

    SPDLOG_INFO("stop test: ExecuteSimpleSQL");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::AddGlobalTestEnvironment(new SmallEnvironment);

    return RUN_ALL_TESTS();
}
