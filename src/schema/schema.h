#pragma once

#include <absl/status/status.h>

namespace schema {

class Column {
   public:
    std::string name;
    std::string type;
    bool primary_key = false;

    Column(const std::string& name, const std::string& type)
        : name(name), type(type) {}

    Column(const std::string& name, const std::string& type, bool primary_key)
        : name(name), type(type), primary_key(primary_key) {}
};

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns);

}  // namespace schema