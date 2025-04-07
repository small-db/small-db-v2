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

#include <fstream>
#include <memory>
#include <string>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/statusor.h"
#include "absl/strings/str_split.h"

// =====================================================================
// self header
// =====================================================================

#include "test/parser/parser.h"

namespace parser {

SQLTestUnit::SQLTestUnit(std::vector<std::string> labels, std::string sql,
                         std::string raw_expected,
                         behaviour_t expected_behavior)
    : labels(labels),
      sql(sql),
      raw_expected(raw_expected),
      expected_behavior(expected_behavior) {}

static absl::StatusOr<std::unique_ptr<SQLTestUnit>> init(
    std::vector<std::string> lines) {
    // this first line is tags <tag1> <tag2>
    if (lines.size() < 2) {
        return absl::InvalidArgumentError(
            "a sql unit must have at least 2 lines");
    }

    auto tags = absl::StrSplit(lines[0], ' ');
    auto sql = lines[1];
    for (int row = 2; row < lines.size(); row++) {
        if (lines[row] == "----") {
            break;
        }
        sql += "\n" + lines[row];
    }

    // statement ok
    SQLTestUnit::behaviour_t behavior;
    if (lines[0] == "statement ok") {
        behavior = SQLTestUnit::StatementOK();
    } else if (*tags.begin() == "query") {
        // query
        auto query = SQLTestUnit::Query();
        auto column_names = absl::StrSplit(lines[0], ' ');
        for (const auto& name : column_names) {
            query.column_names.push_back(name.data());
        }
        for (int row = 2; row < lines.size(); row++) {
            if (lines[row] == "----") {
                break;
            }
            auto values = absl::StrSplit(lines[row], ' ');
            std::vector<std::string> value;
            for (const auto& v : values) {
                // value.push_back(v);
            }
            query.expected_output.push_back(value);
        }
        behavior = query;
    } else {
        return absl::InvalidArgumentError("unknown sql unit");
    }

    auto sql_unit = std::make_unique<SQLTestUnit>(tags, sql, "");
    if (sql_unit->sql.empty()) {
        return absl::InvalidArgumentError("empty sql");
    }
    return sql_unit;
}

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
                auto sql_unit = SQLTestUnit::init(lines);
                if (!sql_unit.ok()) {
                    return sql_unit.status();
                }

                sql_tests.push_back(*sql_unit.value());
                lines.clear();
            }
        } else {
            lines.push_back(line);
        }
    }

    return sql_tests;
}

}  // namespace parser
