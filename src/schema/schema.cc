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

// =====================================================================
// self header
// =====================================================================

#include "src/schema/schema.h"

namespace schema {

void to_json(nlohmann::json& j, const Column& c) {
    j = nlohmann::json{{"name", c.name},
                       {"type", c.type},
                       {"is_primary_key", c.is_primary_key},
                       {"partitioning", c.partitioning},
                       {"partition_values", c.partition_values}};
}

void from_json(const nlohmann::json& j, Column& c) {
    j.at("name").get_to(c.name);
    j.at("type").get_to(c.type);
    j.at("is_primary_key").get_to(c.is_primary_key);
    j.at("partitioning").get_to(c.partitioning);
    j.at("partition_values").get_to(c.partition_values);
}

void to_json(nlohmann::json& j, const Table& t) {
    j = nlohmann::json{{"name", t.name}, {"columns", t.columns}};
}

void from_json(const nlohmann::json& j, Table& t) {
    j.at("name").get_to(t.name);
    j.at("columns").get_to(t.columns);
}

class Catalog {
   private:
    // static pointer to the Singleton instance
    static Catalog* instancePtr;

    // mutex to ensure thread safety
    static std::mutex mtx;

    // private Constructor
    Catalog() {
        std::string db_path = DATA_DIR + "/" + TABLE_TABLES;
        rocks_wrapper::RocksDBWrapper db(
            db_path, {"TablesCF", "ColumnsCF", "PartitionsCF"});

        auto kv_pairs = db.GetAllKV("TablesCF");
        for (const auto& kv : kv_pairs) {
            nlohmann::json j = nlohmann::json::parse(kv.second);
            Table table;
            from_json(j, table);
            tables[table.name] = std::make_shared<Table>(table);
        }
    }

    std::unordered_map<std::string, std::shared_ptr<Table>> tables;

   public:
    /**
     * Delete the assignment operator.
     */
    void operator=(const Catalog&) = delete;

    /**
     * Delete the copy constructor.
     */
    Catalog(const Catalog& obj) = delete;

    // Static method to get the Singleton instance
    static Catalog* getInstance() {
        if (instancePtr == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instancePtr == nullptr) {
                instancePtr = new Catalog();
            }
        }
        return instancePtr;
    }

    std::optional<std::shared_ptr<Table>> get_table(
        const std::string& table_name) {
        auto it = tables.find(table_name);
        if (it != tables.end()) {
            return it->second;
        } else {
            SPDLOG_ERROR("table not found: {}", table_name);
            return std::nullopt;
        }
    }

    absl::Status drop_table(const std::string& table_name) {
        auto it = tables.find(table_name);
        if (it != tables.end()) {
            tables.erase(it);
        }

        std::string db_path = DATA_DIR + "/" + TABLE_TABLES;
        rocks_wrapper::RocksDBWrapper db(
            db_path, {"TablesCF", "ColumnsCF", "PartitionsCF"});
        db.Delete("TablesCF", table_name);
        return absl::OkStatus();
    }

    absl::Status add_table(const std::string& table_name,
                           const std::vector<Column>& columns) {
        auto table = get_table(table_name);
        if (table.has_value()) {
            return absl::AlreadyExistsError("Table already exists");
        }

        std::string db_path = DATA_DIR + "/" + TABLE_TABLES;
        rocks_wrapper::RocksDBWrapper db(
            db_path, {"TablesCF", "ColumnsCF", "PartitionsCF"});

        db.PrintAllKV();

        // store table metadata
        {
            auto table = Table(table_name, columns);

            nlohmann::json j(table);

            auto table_id = id::generate_id();
            auto key = absl::StrFormat("T:%d", table_id);
            db.Put("TablesCF", key, j.dump());

            this->tables[table_name] =
                std::make_shared<Table>(table_name, columns);
        }

        db.PrintAllKV();

        return absl::OkStatus();
    }
};

// define the static members
Catalog* Catalog::instancePtr = nullptr;
std::mutex Catalog::mtx;

// The type must be DefaultConstructible to be converted from JSON.
// (https://github.com/nlohmann/json)
Column::Column() = default;

Column::Column(const std::string& name, const std::string& type)
    : name(name), type(type) {}

void Column::set_primary_key(bool set) { is_primary_key = set; }

void Column::set_partitioning(PgQuery__PartitionStrategy strategy) {
    partitioning = strategy;
}

std::optional<std::shared_ptr<Table>> get_table(const std::string& table_name) {
    return Catalog::getInstance()->get_table(table_name);
}

Table::Table() = default;

Table::Table(const std::string& name, const std::vector<Column>& columns)
    : name(name), columns(columns) {}

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns) {
    return Catalog::getInstance()->add_table(table_name, columns);
}

absl::Status drop_table(const std::string& table_name) {
    return Catalog::getInstance()->drop_table(table_name);
}

absl::Status add_list_partition(const std::string& table_name,
                                const std::string& partition_name,
                                const std::vector<std::string>& values) {
    auto table = get_table(table_name);
    if (!table.has_value()) {
        return absl::NotFoundError("Table not found");
    }

    SPDLOG_INFO("add_list_partition: table_name: {}, partition_name: {}",
                table_name, partition_name);
    for (const auto& value : values) {
        SPDLOG_INFO("value: {}", value);
    }
    return absl::OkStatus();
}

}  // namespace schema
