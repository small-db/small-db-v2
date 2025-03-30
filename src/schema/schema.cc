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
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/status.h"
#include "absl/strings/str_format.h"

// json
#include "nlohmann/json.hpp"

// rocksdb
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/id/generator.h"
#include "src/rocks/wrapper.h"
#include "src/schema/const.h"
#include "src/schema/schema.h"

namespace schema {

Column::Column() = default;

Column::Column(const std::string& name, const std::string& type)
    : name(name), type(type) {}

void Column::set_primary_key(bool set) { is_primary_key = set; }

void Column::set_partitioning(PgQuery__PartitionStrategy strategy) {
    partitioning = strategy;
}

void to_json(nlohmann::json& j, const Column& c) {
    j = nlohmann::json{{"name", c.name},
                       {"type", c.type},
                       {"is_primary_key", c.is_primary_key},
                       {"partitioning", c.partitioning}};
}

void from_json(const nlohmann::json& j, Column& c) {
    j.at("name").get_to(c.name);
    j.at("type").get_to(c.type);
    j.at("is_primary_key").get_to(c.is_primary_key);
    j.at("partitioning").get_to(c.partitioning);
}

std::optional<Table> get_table(const std::string& table_name) {
    return std::nullopt;
}

void scan_all_kv(rocksdb::DB* db) {
    if (db == nullptr) {
        SPDLOG_ERROR("RocksDB instance is null");
        return;
    }

    std::unique_ptr<rocksdb::Iterator> it(
        db->NewIterator(rocksdb::ReadOptions()));
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        SPDLOG_INFO("Key: {}, Value: {}", key, value);
    }

    if (!it->status().ok()) {
        SPDLOG_ERROR("Error during iteration: {}", it->status().ToString());
    }
}

Table::Table(const std::string& name, const std::vector<Column>& columns)
    : name(name), columns(columns) {}

void to_json(nlohmann::json& j, const Table& t) {
    j = nlohmann::json{{"name", t.name}, {"columns", t.columns}};
}

void from_json(const nlohmann::json& j, Table& t) {
    j.at("name").get_to(t.name);
    j.at("columns").get_to(t.columns);
}

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns) {
    auto table = get_table(table_name);
    if (table.has_value()) {
        return absl::AlreadyExistsError("Table already exists");
    }

    // auto db = open_rocksdb(TABLE_TABLES);
    std::string db_path = DATA_DIR + "/" + table_name;
    rocks_wrapper::RocksDBWrapper db(db_path,
                                     {"TablesCF", "ColumnsCF", "PartitionsCF"});

    SPDLOG_INFO("Table DefaultConstructible: {}",
                std::is_default_constructible_v<Table>);
    SPDLOG_INFO("Column DefaultConstructible: {}",
                std::is_default_constructible_v<Column>);

    db.PrintAllKV();

    // store table metadata
    {
        auto table = Table(table_name, columns);

        nlohmann::json j(table);

        auto table_id = id::generate_id();
        auto key = absl::StrFormat("T:%d", table_id);
        db.Put("TablesCF", key, j.dump());
    }

    // // store column metadata
    // for (const auto& column : columns) {
    //     nlohmann::json column_json;
    //     to_json(column_json, column);
    //     auto column_id = id::generate_id();
    //     auto column_key = absl::StrFormat("C:%d", column_id);
    //     db.Put("ColumnsCF", column_key, column_json.dump());
    // }

    db.PrintAllKV();

    return absl::OkStatus();
}
}  // namespace schema
