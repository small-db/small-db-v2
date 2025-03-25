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

#include "src/schema/const.h"
#include "src/schema/schema.h"

namespace schema {

// "schemas" -> "<data_dir>/schemas.parquet"
std::string gen_datafile_path(const std::string& tablename) {
    return DATA_DIR + tablename + ".parquet";
}

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns) {
    auto pool = arrow::default_memory_pool();

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

    // columns for "pg_database"
    PARQUET_THROW_NOT_OK(columns_builder->Append());

    for (const auto& column : columns) {
        PARQUET_THROW_NOT_OK(column_builder->Append());
        PARQUET_THROW_NOT_OK(column_names_builder->Append(column.name));
        PARQUET_THROW_NOT_OK(column_types_builder->Append(column.type));
    }

    std::shared_ptr<arrow::Array> columns_list;
    PARQUET_THROW_NOT_OK(columns_builder->Finish(&columns_list));

    // create the table
    std::shared_ptr<arrow::Schema> schema =
        arrow::schema({arrow::field("name", arrow::utf8()),
                       arrow::field("columns", type_columns)});
    auto table = arrow::Table::Make(schema, {table_names, columns_list});

    // write to disk
    std::shared_ptr<arrow::io::FileOutputStream> outfile;
    PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(
                                         gen_datafile_path(TABLE_TABLES)));
    PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
        *table, arrow::default_memory_pool(), outfile, 300));

    return absl::OkStatus();
}
}  // namespace schema