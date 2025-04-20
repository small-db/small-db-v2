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

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// spdlog
#include "spdlog/spdlog.h"

// json
#include "nlohmann/json.hpp"

// =====================================================================
// local libraries
// =====================================================================

#include "src/server_base/args.h"

// =====================================================================
// self header
// =====================================================================

#include "src/catalog/catalog.h"

namespace small::catalog {

Catalog* Catalog::instancePtr = nullptr;

void Catalog::InitInstance() {
    if (instancePtr == nullptr) {
        instancePtr = new Catalog();
    } else {
        SPDLOG_ERROR("catalog instance already initialized");
    }
}

Catalog* Catalog::GetInstance() {
    if (instancePtr == nullptr) {
        SPDLOG_ERROR("catalog instance not initialized");
        return nullptr;
    }
    return instancePtr;
}

Catalog::Catalog() {
    std::vector<small::schema::Column> columns;
    columns.emplace_back("table_name", small::type::Type::String, true);
    columns.emplace_back("columns", small::type::Type::String);
    this->tables["system.tables"] =
        std::make_shared<small::schema::Table>("system.tables", columns);
    this->system_tables = this->tables["system.tables"];

    columns.clear();
    columns.emplace_back("table_name", small::type::Type::String);
    columns.emplace_back("partition_name", small::type::Type::String, true);
    columns.emplace_back("constraint", small::type::Type::String);
    columns.emplace_back("column_name", small::type::Type::String);
    columns.emplace_back("partition_value", small::type::Type::String);
    this->tables["system.partitions"] =
        std::make_shared<small::schema::Table>("system.partitions", columns);
    this->system_partitions = this->tables["system.partitions"];

    auto info = small::server_base::get_info();
    if (!info.ok()) {
        SPDLOG_ERROR("failed to get server info");
        return;
    }
    std::string db_path = info.value()->db_path;
    this->db = small::rocks::RocksDBWrapper::GetInstance(
        db_path, {"TablesCF", "PartitionCF"});
}

std::optional<std::shared_ptr<small::schema::Table>> Catalog::get_table(
    const std::string& table_name) {
    auto it = tables.find(table_name);
    if (it != tables.end()) {
        return it->second;
    } else {
        return std::nullopt;
    }
}

absl::Status Catalog::create_table(
    const std::string& table_name,
    const std::vector<small::schema::Column>& columns) {
    auto table = get_table(table_name);
    if (table.has_value()) {
        return absl::AlreadyExistsError("Table already exists");
    }

    // write to in-memory cache
    auto new_table =
        std::make_shared<small::schema::Table>(table_name, columns);
    tables[table_name] = new_table;

    // write to disk
    std::vector<small::type::Datum> row;
    row.emplace_back(table_name);
    row.emplace_back(nlohmann::json(columns).dump());

    db->WriteRow(this->system_tables, row);

    return absl::OkStatus();
}

}  // namespace small::catalog
