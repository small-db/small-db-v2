#include <iostream>
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

const string CATALOGS_FILE = "catalogs.parquet";

void init_system_databases() {
  auto type_tables = arrow::map(arrow::utf8(), arrow::uint64());
  std::shared_ptr<arrow::Schema> schema =
      arrow::schema({arrow::field("name", arrow::utf8()),
                     arrow::field("tables", type_tables)});

  arrow::StringBuilder schema_names_builder;
  PARQUET_THROW_NOT_OK(schema_names_builder.Append("some"));
  PARQUET_THROW_NOT_OK(schema_names_builder.Append("string"));
  PARQUET_THROW_NOT_OK(schema_names_builder.Append("content"));
  PARQUET_THROW_NOT_OK(schema_names_builder.Append("in"));
  PARQUET_THROW_NOT_OK(schema_names_builder.Append("rows"));
  std::shared_ptr<arrow::Array> schema_names;
  PARQUET_THROW_NOT_OK(schema_names_builder.Finish(&schema_names));

  arrow::StructBuilder tables_builder(type_tables, NULL, NULL);
  PARQUET_THROW_NOT_OK(tables_builder.Append("some"));

  return arrow::Table::Make(schema, {schema_names, strarray});

  // auto table_name = arrow::Field("name", arrow::utf8());
  // auto table_id = arrow::Field("id", arrow::uint64());
  // auto tables =
  //     arrow::Field("tables", arrow::map(arrow::utf8(), arrow::uint64()));
}

void init_default_database() {}

void init() {
  // create the data directory if it does not exist
  if (access(DATA_DIR.c_str(), F_OK) != 0) {
    if (mkdir(DATA_DIR.c_str(), 0777) != 0) {
      SPDLOG_ERROR("Error creating data directory");
      exit(EXIT_FAILURE);
    }
  }

  if (chdir(DATA_DIR.c_str()) != 0) {
    perror("Error changing directory");
    exit(EXIT_FAILURE);
  }

  // Check if CATALOGS_FILE exists
  if (access(CATALOGS_FILE.c_str(), F_OK) != 0) {
    // File does not exist, initialize the databases
    cout << "Catalogs file not found. Initializing databases..." << endl;
    init_system_databases();
    init_default_database();
  } else {
    cout << "Catalogs file found. No initialization needed." << endl;
  }
}

} // namespace store