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
  // The last argument to the function call is the size of the RowGroup in
  // the parquet file. Normally you would choose this to be rather large but
  // for the example, we use a small value to have multiple RowGroups.
  PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
      *table, arrow::default_memory_pool(), outfile, 30));
}

void init_tables() {
  // schema for "database tables"
  auto type_columns = arrow::struct_({arrow::field("name", arrow::utf8()),
                                      arrow::field("type", arrow::uint8())});
  std::shared_ptr<arrow::Schema> schema =
      arrow::schema({arrow::field("name", arrow::utf8()),
                     arrow::field("columns", type_columns)});

  // schema.names for "database tables"
  arrow::StringBuilder table_names_builder;
  PARQUET_THROW_NOT_OK(table_names_builder.Append("pg_database"));
  std::shared_ptr<arrow::Array> table_names;
  PARQUET_THROW_NOT_OK(table_names_builder.Finish(&table_names));

  // schema.columns for "database tables"
  std::shared_ptr<arrow::Array> columns;

  // create the table
  auto table = arrow::Table::Make(schema, {table_names, columns});
  SPDLOG_INFO("generated table:\n{}", table->ToString());

  // write to disk
  std::shared_ptr<arrow::io::FileOutputStream> outfile;
  PARQUET_ASSIGN_OR_THROW(outfile, arrow::io::FileOutputStream::Open(
                                       gen_datafile_path(TABLE_SCHEMAS)));
  // The last argument to the function call is the size of the RowGroup in
  // the parquet file. Normally you would choose this to be rather large but
  // for the example, we use a small value to have multiple RowGroups.
  PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
      *table, arrow::default_memory_pool(), outfile, 30));
}

void read_whole_file() {
  SPDLOG_INFO("reading {} at once", TABLE_SCHEMAS);
  std::shared_ptr<arrow::io::ReadableFile> infile;
  PARQUET_ASSIGN_OR_THROW(
      infile, arrow::io::ReadableFile::Open(TABLE_SCHEMAS,
                                            arrow::default_memory_pool()));

  std::unique_ptr<parquet::arrow::FileReader> reader;
  PARQUET_THROW_NOT_OK(
      parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));
  std::shared_ptr<arrow::Table> table;
  PARQUET_THROW_NOT_OK(reader->ReadTable(&table));
  SPDLOG_INFO("loaded {} rows in {} columns", table->num_rows(),
              table->num_columns());
}

void init_default_database() {}

// search for data files with prefix "tablename-"
bool table_exists(const string &tablename) {
  return false;

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
    init_default_database();
  }

  exit(0);
}

} // namespace store