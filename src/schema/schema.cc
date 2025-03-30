#include <absl/status/status.h>

// arrow core
#include <arrow/api.h>
#include <arrow/io/api.h>

// arrow acero
#include <arrow/acero/exec_plan.h>

// arrow compute
#include <arrow/compute/api.h>
#include <arrow/compute/api_vector.h>
#include <arrow/compute/cast.h>

// arrow dataset
#include <arrow/dataset/dataset.h>
#include <arrow/dataset/file_base.h>
#include <arrow/dataset/file_parquet.h>
#include <arrow/dataset/plan.h>
#include <arrow/dataset/scanner.h>

// arrow util
#include <arrow/util/future.h>
#include <arrow/util/range.h>
#include <arrow/util/thread_pool.h>
#include <arrow/util/vector.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

// spdlog
#include <spdlog/spdlog.h>

// absl
#include <absl/strings/str_format.h>

// rocksdb
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>

// json
#include <nlohmann/json.hpp>

#include "src/id/generator.h"
#include "src/rocks/wrapper.h"
#include "src/schema/const.h"
#include "src/schema/schema.h"

namespace schema {

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