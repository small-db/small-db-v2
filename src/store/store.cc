#include <iostream>
#include <spdlog/spdlog.h>
#include <sys/stat.h> // For mkdir
#include <unistd.h>   // For chdir

using namespace std;

namespace store {

const string DATA_DIR = "data/";

const string CATALOGS_FILE = "catalogs.parquet";

void init_system_databases() {}

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