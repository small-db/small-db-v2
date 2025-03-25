#pragma once

#include "absl/status/status.h"

namespace schema {

struct Column {
    std::string name;
    std::string type;
};

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns);

}  // namespace schema