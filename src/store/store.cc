#include <dirent.h> // For opendir, readdir, closedir
#include <filesystem>
#include <iomanip>
#include <spdlog/spdlog.h>
#include <sys/stat.h> // For mkdir
#include <unistd.h>   // For chdir

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>

using namespace std;

namespace store {

const string DATA_DIR = "data/";
const string TABLE_SCHEMAS = "schemas";
const string TABLE_TABLES = "tables";

const uint8_t TYPE_STRING = 20;

// "schemas" -> "schemas-2021-09-01-12-00-00.parquet"
string gen_datafile_path(const string &tablename) {
  // Get the current time
  auto now = chrono::system_clock::now();
  auto now_time_t = chrono::system_clock::to_time_t(now);
  auto now_ns =
      chrono::duration_cast<chrono::nanoseconds>(now.time_since_epoch())
          .count() %
      1000000000;

  // Format the time into a string
  stringstream ss;
  ss << put_time(localtime(&now_time_t), "%Y-%m-%d-%H-%M-%S") << "-" << setw(9)
     << setfill('0') << now_ns;

  // Generate the file path
  string filepath = tablename + "-" + ss.str() + ".parquet";
  SPDLOG_INFO("generated file path: {}", filepath);
  return filepath;
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
  SPDLOG_INFO("generated table:\n{}", table->ToString());

  // write to disk
  std::shared_ptr<arrow::io::FileOutputStream> outfile;
  PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(
                                       gen_datafile_path(TABLE_SCHEMAS)));
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
  shared_ptr<arrow::Array> table_names;
  PARQUET_THROW_NOT_OK(table_name_builder.Finish(&table_names));

  // columns
  auto column_names_builder = std::make_shared<arrow::StringBuilder>();
  auto column_types_builder = std::make_shared<arrow::UInt8Builder>();
  std::vector<std::shared_ptr<arrow::ArrayBuilder>> field_builders = {
      column_names_builder, column_types_builder};
  auto column_builder =
      make_shared<arrow::StructBuilder>(type_column, pool, field_builders);
  auto columns_builder = make_shared<arrow::ListBuilder>(pool, column_builder);

  // columns for "pg_database"
  PARQUET_THROW_NOT_OK(columns_builder->Append());

  // column "datname"
  PARQUET_THROW_NOT_OK(column_builder->Append());
  PARQUET_THROW_NOT_OK(column_names_builder->Append("datname"));
  PARQUET_THROW_NOT_OK(column_types_builder->Append(TYPE_STRING));

  // column "datcollate"
  PARQUET_THROW_NOT_OK(column_builder->Append());
  PARQUET_THROW_NOT_OK(column_names_builder->Append("datcollate"));
  PARQUET_THROW_NOT_OK(column_types_builder->Append(TYPE_STRING));

  // column "datctype"
  PARQUET_THROW_NOT_OK(column_builder->Append());
  PARQUET_THROW_NOT_OK(column_names_builder->Append("datctype"));
  PARQUET_THROW_NOT_OK(column_types_builder->Append(TYPE_STRING));

  // column "datacl"
  PARQUET_THROW_NOT_OK(column_builder->Append());
  PARQUET_THROW_NOT_OK(column_names_builder->Append("datacl"));
  PARQUET_THROW_NOT_OK(column_types_builder->Append(TYPE_STRING));

  std::shared_ptr<arrow::Array> columns_list;
  PARQUET_THROW_NOT_OK(columns_builder->Finish(&columns_list));

  // create the table
  std::shared_ptr<arrow::Schema> schema =
      arrow::schema({arrow::field("name", arrow::utf8()),
                     arrow::field("columns", type_columns)});
  auto table = arrow::Table::Make(schema, {table_names, columns_list});
  SPDLOG_INFO("generated table:\n{}", table->ToString());

  // write to disk
  std::shared_ptr<arrow::io::FileOutputStream> outfile;
  PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(
                                       gen_datafile_path(TABLE_TABLES)));
  PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
      *table, arrow::default_memory_pool(), outfile, 300));
}

void init_default_database() {}

// search for data files with prefix "tablename-"
bool table_exists(const string &tablename) {
  for (const auto &entry : filesystem::directory_iterator(".")) {
    auto entry_name = entry.path().filename().string();
    string prefix = tablename + "-";
    if (strncmp(entry_name.c_str(), prefix.c_str(), prefix.size()) == 0) {
      return true;
    }
  }
  return false;
}

void init() {
  // create the data directory if it does not exist
  if (access(DATA_DIR.c_str(), F_OK) != 0) {
    if (mkdir(DATA_DIR.c_str(), 0777) != 0) {
      SPDLOG_ERROR("error creating directory {}", DATA_DIR);
      exit(EXIT_FAILURE);
    }
  }

  int ret = chdir(DATA_DIR.c_str());
  if (ret != 0) {
    SPDLOG_ERROR("error changing to directory {}, error: {}", DATA_DIR, ret);
    exit(EXIT_FAILURE);
  }

  // Check if CATALOGS_FILE exists
  if (table_exists(TABLE_SCHEMAS)) {
    SPDLOG_INFO("found existing table schemas");
  } else {
    SPDLOG_INFO("table schemas not found, initializing system databases");
    init_schemas();
    init_tables();
    init_default_database();
  }

  exit(0);
}

} // namespace store