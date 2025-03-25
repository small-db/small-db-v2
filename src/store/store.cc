#include <dirent.h>  // For opendir, readdir, closedir
#include <spdlog/spdlog.h>
#include <sys/stat.h>  // For mkdir
#include <unistd.h>    // For chdir

#include <filesystem>
#include <iomanip>

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

#include "src/schema/const.h"

namespace store {

// "schemas" -> "schemas-2021-09-01-12-00-00.parquet"
std::string gen_datafile_path(const std::string& tablename) {
    // Get the current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      now.time_since_epoch())
                      .count() %
                  1000000000;

    // Format the time into a std::string
    std::stringstream ss;
    ss << std::put_time(localtime(&now_time_t), "%Y-%m-%d-%H-%M-%S") << "-"
       << std::setw(9) << std::setfill('0') << now_ns;

    // Generate the file path
    std::string filepath = tablename + "-" + ss.str() + ".parquet";
    SPDLOG_INFO("generated file path: {}", filepath);
    return filepath;
}

arrow::Status execute_plan_and_collect_as_table(
    arrow::acero::Declaration plan) {
    // collect sink_reader into a Table
    std::shared_ptr<arrow::Table> response_table;
    ARROW_ASSIGN_OR_RAISE(response_table,
                          arrow::acero::DeclarationToTable(std::move(plan)));

    SPDLOG_INFO("results : {}", response_table->ToString());
    return arrow::Status::OK();
}

arrow::Status scan_sink_example(std::shared_ptr<arrow::Table> table) {
    // ensure arrow::dataset node factories are in the registry
    arrow::dataset::internal::Initialize();

    auto dataset = std::make_shared<arrow::dataset::InMemoryDataset>(table);

    auto options = std::make_shared<arrow::dataset::ScanOptions>();
    options->projection =
        arrow::compute::project({}, {});  // create empty projection

    // construct the scan node
    auto scan_node_options = arrow::dataset::ScanNodeOptions{dataset, options};

    arrow::acero::Declaration scan{"scan", std::move(scan_node_options)};

    return execute_plan_and_collect_as_table(std::move(scan));
}

void init_schemas() {
    // schema for "database schemas"
    auto type_tables = arrow::map(arrow::utf8(), arrow::uint64());
    std::shared_ptr<arrow::Schema> schema =
        arrow::schema({arrow::field("name", arrow::utf8()),
                       arrow::field("tables", type_tables)});

    // schema.names for "database schemas"
    auto schema_names_builder = arrow::StringBuilder();
    PARQUET_THROW_NOT_OK(schema_names_builder.Append("pg_catalog"));
    std::shared_ptr<arrow::Array> schema_names;
    PARQUET_THROW_NOT_OK(schema_names_builder.Finish(&schema_names));

    // schema.tables for "database schemas"
    auto keys_builder = std::make_shared<arrow::StringBuilder>();
    auto values_builder = std::make_shared<arrow::UInt64Builder>();
    arrow::MapBuilder tables_builder(arrow::default_memory_pool(), keys_builder,
                                     values_builder);
    PARQUET_THROW_NOT_OK(tables_builder.Append());
    PARQUET_THROW_NOT_OK(
        keys_builder->AppendScalar(arrow::StringScalar("pg_database")));
    PARQUET_THROW_NOT_OK(values_builder->AppendScalar(arrow::UInt64Scalar(1)));
    std::shared_ptr<arrow::Array> tables;
    PARQUET_THROW_NOT_OK(tables_builder.Finish(&tables));

    // create the table
    auto table = arrow::Table::Make(schema, {schema_names, tables});

    // write to disk
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile,
                            arrow::io::FileOutputStream::Open(
                                gen_datafile_path(schema::TABLE_SCHEMAS)));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
        *table, arrow::default_memory_pool(), outfile, 300));
}

void init_tables() {
    auto pool = arrow::default_memory_pool();

    // declare custom types
    auto type_column_name = arrow::utf8();
    auto type_column_type = arrow::uint8();
    auto type_column = arrow::struct_({arrow::field("name", type_column_name),
                                       arrow::field("type", type_column_type)});
    auto type_columns = arrow::list(type_column);

    // names
    auto table_name_builder = arrow::StringBuilder();
    PARQUET_THROW_NOT_OK(table_name_builder.Append("pg_database"));
    std::shared_ptr<arrow::Array> table_names;
    PARQUET_THROW_NOT_OK(table_name_builder.Finish(&table_names));

    // columns
    auto column_names_builder = std::make_shared<arrow::StringBuilder>();
    auto column_types_builder = std::make_shared<arrow::UInt8Builder>();
    std::vector<std::shared_ptr<arrow::ArrayBuilder>> field_builders = {
        column_names_builder, column_types_builder};
    auto column_builder =
        make_shared<arrow::StructBuilder>(type_column, pool, field_builders);
    auto columns_builder =
        make_shared<arrow::ListBuilder>(pool, column_builder);

    // columns for "pg_database"
    PARQUET_THROW_NOT_OK(columns_builder->Append());

    // column "datname"
    PARQUET_THROW_NOT_OK(column_builder->Append());
    PARQUET_THROW_NOT_OK(column_names_builder->Append("datname"));
    PARQUET_THROW_NOT_OK(column_types_builder->Append(schema::TYPE_STRING));

    // column "datcollate"
    PARQUET_THROW_NOT_OK(column_builder->Append());
    PARQUET_THROW_NOT_OK(column_names_builder->Append("datcollate"));
    PARQUET_THROW_NOT_OK(column_types_builder->Append(schema::TYPE_STRING));

    // column "datctype"
    PARQUET_THROW_NOT_OK(column_builder->Append());
    PARQUET_THROW_NOT_OK(column_names_builder->Append("datctype"));
    PARQUET_THROW_NOT_OK(column_types_builder->Append(schema::TYPE_STRING));

    // column "datacl"
    PARQUET_THROW_NOT_OK(column_builder->Append());
    PARQUET_THROW_NOT_OK(column_names_builder->Append("datacl"));
    PARQUET_THROW_NOT_OK(column_types_builder->Append(schema::TYPE_STRING));

    std::shared_ptr<arrow::Array> columns_list;
    PARQUET_THROW_NOT_OK(columns_builder->Finish(&columns_list));

    // create the table
    std::shared_ptr<arrow::Schema> schema =
        arrow::schema({arrow::field("name", arrow::utf8()),
                       arrow::field("columns", type_columns)});
    auto table = arrow::Table::Make(schema, {table_names, columns_list});

    // write to disk
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile,
                            arrow::io::FileOutputStream::Open(
                                gen_datafile_path(schema::TABLE_TABLES)));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
        *table, arrow::default_memory_pool(), outfile, 300));

    arrow::Status status = scan_sink_example(table);

    if (!status.ok()) {
        SPDLOG_ERROR("error occurred: {}", status.message());
    }
}

void init_demo() {
    // i
    auto i_builder = arrow::Int64Builder();
    PARQUET_THROW_NOT_OK(i_builder.Append(101));
    PARQUET_THROW_NOT_OK(i_builder.Append(102));
    PARQUET_THROW_NOT_OK(i_builder.Append(103));
    PARQUET_THROW_NOT_OK(i_builder.Append(104));
    std::shared_ptr<arrow::Array> i_list;
    PARQUET_THROW_NOT_OK(i_builder.Finish(&i_list));

    // b
    auto b_builder = arrow::BooleanBuilder();
    PARQUET_THROW_NOT_OK(b_builder.Append(true));
    PARQUET_THROW_NOT_OK(b_builder.Append(false));
    PARQUET_THROW_NOT_OK(b_builder.Append(true));
    PARQUET_THROW_NOT_OK(b_builder.Append(false));
    std::shared_ptr<arrow::Array> b_list;
    PARQUET_THROW_NOT_OK(b_builder.Finish(&b_list));

    // create the table
    std::shared_ptr<arrow::Schema> schema =
        arrow::schema({arrow::field("i", arrow::int64()),
                       arrow::field("b", arrow::boolean())});
    auto table = arrow::Table::Make(schema, {i_list, b_list});

    // write to disk
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile,
                            arrow::io::FileOutputStream::Open("demo.parquet"));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
        *table, arrow::default_memory_pool(), outfile, 300));

    // arrow::Status status = scan_sink_example(table);

    // if (!status.ok()) {
    //   SPDLOG_ERROR("error occurred: {}", status.message());
    // }
}

// search for data files with prefix "tablename-"
bool table_exists(const std::string& tablename) {
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        auto entry_name = entry.path().filename().string();
        std::string prefix = tablename + "-";
        if (strncmp(entry_name.c_str(), prefix.c_str(), prefix.size()) == 0) {
            return true;
        }
    }
    return false;
}

void init() {
    // create the data directory if it does not exist
    if (access(schema::DATA_DIR.c_str(), F_OK) != 0) {
        if (mkdir(schema::DATA_DIR.c_str(), 0777) != 0) {
            SPDLOG_ERROR("error creating directory {}", schema::DATA_DIR);
            exit(EXIT_FAILURE);
        }
    }

    return;

    int ret = chdir(schema::DATA_DIR.c_str());
    if (ret != 0) {
        SPDLOG_ERROR("error changing to directory {}, error: {}",
                     schema::DATA_DIR, ret);
        exit(EXIT_FAILURE);
    }

    if (!table_exists(schema::TABLE_SCHEMAS)) {
        init_schemas();
    }

    if (!table_exists(schema::TABLE_TABLES)) {
        init_tables();
    }

    // init_demo();
}

}  // namespace store