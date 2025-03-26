#pragma once

#include <absl/status/status.h>

namespace schema {

enum PartitioningType {
    NONE,
    LIST,
    RANGE,
    HASH,
};

class Column {
   public:
    std::string name;
    std::string type;
    bool is_primary_key = false;
    PartitioningType partitioning = PartitioningType::NONE;

    Column(const std::string& name, const std::string& type)
        : name(name), type(type) {}

    void set_primary_key(bool set) { is_primary_key = set; }

    void set_partitioning(PartitioningType type) { partitioning = type; }
};

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns);

}  // namespace schema