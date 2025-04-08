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

// =====================================================================
// c++ std
// =====================================================================

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"

// pqxx
#include "pqxx/pqxx"

// gtest
#include "gtest/gtest.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/base/base.h"
#include "src/server/server.h"
#include "src/type/type.h"
#include "test/parser/parser.h"

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

constexpr std::string_view CONNECTION_STRING =
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

        pqxx::connection conn = pqxx::connection{CONNECTION_STRING.data()};
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

absl::Status run_sql_test(const std::string& sqltest_file) {
    auto sql_units = parser::read_sql_test(sqltest_file);
    if (!sql_units.ok()) {
        return sql_units.status();
    }

    // print the sql units
    for (const auto& unit : sql_units.value()) {
        SPDLOG_INFO("SQL Test Unit:");
        for (const auto& line : unit.labels) {
            SPDLOG_INFO("Label: {}", line);
        }
        SPDLOG_INFO("SQL: {}", unit.sql);
    }

    return absl::OkStatus();
}

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) {
    auto status = run_sql_test("test/integration_test/test.sqltest");
    if (!status.ok()) {
        SPDLOG_ERROR("Test failed with status: {}", status.ToString());
    }
    ASSERT_TRUE(status.ok());
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::AddGlobalTestEnvironment(new SmallEnvironment);

    return RUN_ALL_TESTS();
}
