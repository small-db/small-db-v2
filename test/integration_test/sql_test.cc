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

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <pqxx/pqxx>
#include <string>

#include "src/base/base.h"
#include "src/server/server.h"

#include <gtest/gtest.h>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <absl/strings/str_format.h>

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

class SQLTestCase {
   public:
    std::string sql;
    std::string expected_output;
};

std::vector<SQLTestCase> read_cases(const std::string& file_path) {
    std::vector<SQLTestCase> cases;

    std::ifstream file(file_path);
    if (!file.is_open()) {
        SPDLOG_ERROR("failed to open file: {}", file_path);
        return cases;
    }

    std::ostringstream currentSQL;
    std::string line;
    bool emptyBlock = true;

    while (std::getline(file, line)) {
        if (line.empty()) {
            if (!emptyBlock && !currentSQL.str().empty()) {
                cases.push_back({currentSQL.str(), ""});
                currentSQL.str("");
                currentSQL.clear();
                emptyBlock = true;
            }
        } else {
            currentSQL << line << '\n';
            emptyBlock = false;
        }
    }

    // Add the last SQL if there's no trailing empty line
    if (!currentSQL.str().empty()) {
        cases.push_back({currentSQL.str(), ""});
    }

    return cases;
}

class SQLTestUnit {
   private:
   public:
    std::vector<std::string> labels;
    std::string sql;
    std::string expected_raw;
    SQLTestUnit(std::vector<std::string> lines) {
        for (const auto& line : lines) {
            if (line.empty()) continue;

            if (line[0] == '#') {
                labels.push_back(line.substr(1));
            } else {
                sql += line + "\n";
            }
        }
    }
};

// absl::Status run_sql_test(const std::string& sqltest_file) {

absl::StatusOr<std::vector<SQLTestUnit>> read_sql_test(
    const std::string& sqltest_file) {
    std::vector<SQLTestUnit> sql_tests;
    std::ifstream file(sqltest_file);
    if (!file.is_open()) {
        return absl::NotFoundError(
            absl::StrFormat("failed to open file: %s", sqltest_file));
    }

    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        if (line.empty()) {
            if (!lines.empty()) {
                sql_tests.emplace_back(lines);
                lines.clear();
            }
        } else {
            lines.push_back(line);
        }
    }

    return sql_tests;
}

absl::Status run_sql_test(const std::string& sqltest_file) {
    auto sql_units = read_sql_test(sqltest_file);
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
    auto status = run_sql_test("test/integration_test/test.sql");
    ASSERT_TRUE(status.ok());

    // SPDLOG_INFO("start test: ExecuteSimpleSQL");

    // pqxx::connection conn = pqxx::connection{CONNECTION_STRING};

    // pqxx::work tx(conn);

    // std::string sql_file_path = "test/integration_test/test.sql";

    // auto cases = read_cases(sql_file_path);
    // for (auto& c : cases) {
    //     SPDLOG_INFO("executing SQL: {}", c.sql);

    //     pqxx::result r = tx.exec(c.sql);

    //     for (auto row : r) {
    //         for (auto field : row) {
    //             SPDLOG_INFO("field: {}", field.c_str());
    //         }
    //     }
    // }

    // tx.commit();

    // SPDLOG_INFO("stop test: ExecuteSimpleSQL");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);

    testing::AddGlobalTestEnvironment(new SmallEnvironment);

    return RUN_ALL_TESTS();
}
