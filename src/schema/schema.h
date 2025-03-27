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

    Column(const std::string& name, const std::string& type);

    void set_primary_key(bool set);

    void set_partitioning(PgQuery__PartitionStrategy strategy);
};

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns);

}  // namespace schema