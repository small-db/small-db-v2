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
#include <spdlog/spdlog.h>

// rocksdb
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>

#include "src/schema/const.h"
#include "src/schema/schema.h"

namespace schema {

Column::Column(const std::string& name, const std::string& type)
    : name(name), type(type) {}

void Column::set_primary_key(bool set) { is_primary_key = set; }

void Column::set_partitioning(PgQuery__PartitionStrategy strategy) {
    partitioning = strategy;
}

// "schemas" -> "<data_dir>/schemas.parquet"
std::string gen_datafile_path(const std::string& tablename) {
    return DATA_DIR + tablename + ".parquet";
}

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns) {
    rocksdb::DB* db;
    rocksdb::Options options;

    // Optimize RocksDB. This is the easiest way to get RocksDB to perform well.
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();

    // Create the DB if it's not already present.
    options.create_if_missing = true;

    std::string DBPath = DATA_DIR + TABLE_TABLES;
    rocksdb::Status s = rocksdb::DB::Open(options, DBPath, &db);
    if (!s.ok()) {
        return absl::InternalError("Failed to open RocksDB");
    }

    // Put key-value
    // s = db->Put(rocksdb::WriteOptions(), "key1", "value");

    // get value
    std::string value;
    s = db->Get(rocksdb::ReadOptions(), "key1", &value);
    SPDLOG_INFO("value: {}", value);

    return absl::OkStatus();

    auto pool = arrow::default_memory_pool();

    if (columns.empty()) {
        return absl::OkStatus();
    }

    // declare custom types
    auto type_column_name = arrow::utf8();
    auto type_column_type = arrow::utf8();
    auto type_column = arrow::struct_({arrow::field("name", type_column_name),
                                       arrow::field("type", type_column_type)});
    auto type_columns = arrow::list(type_column);

    // names
    auto table_name_builder = arrow::StringBuilder();
    PARQUET_THROW_NOT_OK(table_name_builder.Append(table_name));
    std::shared_ptr<arrow::Array> table_names;
    PARQUET_THROW_NOT_OK(table_name_builder.Finish(&table_names));

    // columns
    auto column_names_builder = std::make_shared<arrow::StringBuilder>();
    auto column_types_builder = std::make_shared<arrow::StringBuilder>();
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> field_builders = {
        column_names_builder, column_types_builder};
    auto column_builder =
        make_shared<arrow::StructBuilder>(type_column, pool, field_builders);
    auto columns_builder =
        make_shared<arrow::ListBuilder>(pool, column_builder);

    PARQUET_THROW_NOT_OK(columns_builder->Append());

    for (const auto& column : columns) {
        PARQUET_THROW_NOT_OK(column_builder->Append());
        PARQUET_THROW_NOT_OK(column_names_builder->Append(column.name));
        PARQUET_THROW_NOT_OK(column_types_builder->Append(column.type));
    }

    std::shared_ptr<arrow::Array> columns_list;
    PARQUET_THROW_NOT_OK(columns_builder->Finish(&columns_list));

    // primary key
    std::string pk_name;
    for (const auto& column : columns) {
        if (column.is_primary_key) {
            pk_name = column.name;
            break;
        }
    }
    if (pk_name.empty()) {
        return absl::InvalidArgumentError("No primary key found");
    }
    auto pk_builder = arrow::StringBuilder();
    PARQUET_THROW_NOT_OK(pk_builder.Append(pk_name));
    std::shared_ptr<arrow::Array> pk_names;
    PARQUET_THROW_NOT_OK(pk_builder.Finish(&pk_names));

    // partition key
    auto type_partition_key =
        arrow::struct_({arrow::field("name", arrow::utf8()),
                        arrow::field("strategy", arrow::int8())});
    auto partition_name_builder = std::make_shared<arrow::StringBuilder>();
    auto partition_strategy_builder = std::make_shared<arrow::Int8Builder>();
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> partition_field_builders =
        {partition_name_builder, partition_strategy_builder};
    auto partition_key_builder = make_shared<arrow::StructBuilder>(
        type_partition_key, pool, partition_field_builders);

    std::string partition_name;
    PgQuery__PartitionStrategy partition_strategy;
    for (const auto& column : columns) {
        if (column.partitioning !=
            PG_QUERY__PARTITION_STRATEGY__PARTITION_STRATEGY_UNDEFINED) {
            partition_name = column.name;
            partition_strategy = column.partitioning;
            break;
        }
    }

    PARQUET_THROW_NOT_OK(partition_key_builder->Append());
    PARQUET_THROW_NOT_OK(partition_name_builder->Append(partition_name));
    PARQUET_THROW_NOT_OK(
        partition_strategy_builder->Append(partition_strategy));

    // create the table
    std::shared_ptr<arrow::Schema> schema =
        arrow::schema({arrow::field("name", arrow::utf8()),
                       arrow::field("columns", type_columns),
                       arrow::field("primary_key", arrow::utf8()),
                       arrow::field("partition_key", type_partition_key)});
    auto table =
        arrow::Table::Make(schema, {table_names, columns_list, pk_names});

    // write to disk
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(
                                         gen_datafile_path(TABLE_TABLES)));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
        *table, arrow::default_memory_pool(), outfile, 300));

    return absl::OkStatus();
}
}  // namespace schema