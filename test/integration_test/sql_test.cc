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
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <variant>
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

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/server/server.h"
#include "src/type/type.h"
#include "test/parser/parser.h"

class SmallEnvironment : public ::testing::Environment {
   public:
    ~SmallEnvironment() override {}

    // Override this to define how to set up the environment.
    void SetUp() override {
        spdlog::set_level(spdlog::level::debug);
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");

        SPDLOG_INFO("setting up the environment");
    }

    // Override this to define how to tear down the environment.
    void TearDown() override { SPDLOG_INFO("tearing down the environment"); }
};

constexpr std::string_view CONNECTION_STRING =
    "dbname=postgres user=postgres password=postgres hostaddr=127.0.0.1 "
    "port=5001";

// Test fixture for setting up and tearing down the server
class SQLTest : public ::testing::Test {
   protected:
    static void SetUpTestSuite() {
        SPDLOG_INFO("starting the server");
        StartServers();
        WaitServer();
    }

    static std::vector<std::thread> server_threads;

    static void StartServers() {
        SPDLOG_INFO("starting the server thread");

        std::vector<server::ServerArgs> server_args = {
            {5001, 50001, "asia", "", "./data/asia"},
            {5002, 50002, "eu", "127.0.0.1:50001", "./data/eu"},
            // {5003, 50003, "us", "127.0.0.1:50001", "./data/us"},
        };

        // server::RunServer(server_args[0]);

        for (auto& args : server_args) {
            server_threads.emplace_back(server::RunServer, args);
        }
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

        for (auto& server_thread : server_threads) {
            if (server_thread.joinable()) {
                server_thread.join();
            }
        }
    }
};

std::vector<std::thread> SQLTest::server_threads;

absl::Status run_sql_test(const std::string& sqltest_file) {
    auto sql_units = parser::read_sql_test(sqltest_file);
    if (!sql_units.ok()) {
        return sql_units.status();
    }

    pqxx::connection conn{CONNECTION_STRING.data()};

    // print the sql units
    for (const auto& unit : sql_units.value()) {
        if (auto stmtOK = std::get_if<parser::SQLTestUnit::StatementOK>(
                &unit.expected_behavior)) {
            pqxx::work tx(conn);
            pqxx::result r = tx.exec(unit.sql);
            tx.commit();
        } else if (auto query = std::get_if<parser::SQLTestUnit::Query>(
                       &unit.expected_behavior)) {
            pqxx::work tx(conn);
            pqxx::result r = tx.exec(unit.sql);

            // check schema (row description)
            {
                // check column count
                if (r.columns() != query->column_names.size()) {
                    return absl::InvalidArgumentError(absl::StrFormat(
                        "column count mismatch: expected %d, got %d",
                        query->column_names.size(), r.columns()));
                }

                // check column names
                for (int i = 0; i < r.columns(); ++i) {
                    if (r.column_name(i) != query->column_names[i]) {
                        return absl::InvalidArgumentError(absl::StrFormat(
                            "column name mismatch: expected %s, got %s",
                            query->column_names[i], r.column_name(i)));
                    }
                }

                // check column types
                for (int i = 0; i < r.columns(); ++i) {
                    if (type::from_pgwire_oid(r.column_type(i)).value() !=
                        query->column_types[i]) {
                        return absl::InvalidArgumentError(absl::StrFormat(
                            "column type mismatch: expected %s, got %s",
                            typeid(query->column_types[i]).name(),
                            typeid(r.column_type(i)).name()));
                    }
                }
            }

            // check data
            {
                // check row count
                if (r.size() != query->expected_output.size()) {
                    return absl::InvalidArgumentError(absl::StrFormat(
                        "row count mismatch: expected %d, got %d",
                        query->expected_output.size(), r.size()));
                }

                // check data
                for (int i = 0; i < r.size(); ++i) {
                    for (int j = 0; j < r.columns(); ++j) {
                        if (r[i][j].c_str() != query->expected_output[i][j]) {
                            return absl::InvalidArgumentError(absl::StrFormat(
                                "data mismatch at row %d, column %d: "
                                "expected %s, got %s",
                                i, j, query->expected_output[i][j],
                                r[i][j].c_str()));
                        }
                    }
                }
            }

            tx.commit();
        }
    }

    return absl::OkStatus();
}

// Test case to execute simple SQL commands
TEST_F(SQLTest, ExecuteSimpleSQL) {
    ASSERT_NO_THROW({
        auto status = run_sql_test("test/integration_test/test.sqltest");
        if (!status.ok()) {
            SPDLOG_ERROR("Test failed with status: {}", status.ToString());
        }

        ASSERT_TRUE(status.ok());
    });
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::AddGlobalTestEnvironment(new SmallEnvironment);

    return RUN_ALL_TESTS();
}
