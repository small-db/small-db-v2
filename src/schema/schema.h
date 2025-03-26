#pragma once

#include <absl/status/status.h>
#include <pg_query.h>
#include <pg_query.pb-c.h>

namespace schema {

class Column {
   public:
    std::string name;
    std::string type;
    bool is_primary_key = false;
    PgQuery__PartitionStrategy partitioning =
        PG_QUERY__PARTITION_STRATEGY__PARTITION_STRATEGY_UNDEFINED;

    Column(const std::string& name, const std::string& type)
        : name(name), type(type) {}

    void set_primary_key(bool set) { is_primary_key = set; }

    void set_partitioning(PartitioningType type) { partitioning = type; }
};

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns);

}  // namespace schema