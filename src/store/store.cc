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
  // schema for "database schemas"
  auto type_tables = arrow::map(arrow::utf8(), arrow::uint64());
  std::shared_ptr<arrow::Schema> schema =
      arrow::schema({arrow::field("name", arrow::utf8()),
                     arrow::field("tables", type_tables)});

  // schema.names for "database schemas"
  arrow::StringBuilder schema_names_builder;
  PARQUET_THROW_NOT_OK(schema_names_builder.Append("pg_catalog"));
  std::shared_ptr<arrow::Array> schema_names;
  PARQUET_THROW_NOT_OK(schema_names_builder.Finish(&schema_names));

  // schema.tables for "database schemas"
  arrow::StringBuilder key_builder;
  arrow::UInt64Builder value_builder;
  vector<shared_ptr<arrow::ArrayBuilder>> field_builders;

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
  SPDLOG_INFO("Generated table:\n{}", table->ToString());

  // write to disk
  std::shared_ptr<arrow::io::FileOutputStream> outfile;
  PARQUET_ASSIGN_OR_THROW(outfile,
                          arrow::io::FileOutputStream::Open(CATALOGS_FILE));
  // The last argument to the function call is the size of the RowGroup in
  // the parquet file. Normally you would choose this to be rather large but
  // for the example, we use a small value to have multiple RowGroups.
  PARQUET_THROW_NOT_OK(parquet::arrow::WriteTable(
      *table, arrow::default_memory_pool(), outfile, 3));
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
    SPDLOG_INFO("Catalogs file not found. Initializing databases...");
    init_system_databases();
    init_default_database();
  } else {
    SPDLOG_INFO("Catalogs file found. No initialization needed.");
  }
}

} // namespace store