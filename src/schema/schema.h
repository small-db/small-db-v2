// Copyright 2025 Xiaochen Cui
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The storage pattern of metadata is:
//
// - table metadata
//   - key: T:<table ID>
//   - value: <table metadata>
// - column metadata
//   - key: C:<table ID>:<column ID>
//   - value: <column metadata>
// - partition metadata
//   - key: P:<table ID>:<partition ID>
//   - value: <partition metadata>

#pragma once

// =====================================================================
// c++ std
// =====================================================================

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/status.h"

// pg_query
#include "pg_query.h"
#include "pg_query.pb-c.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/schema/partition.h"
#include "src/type/type.h"

namespace schema {

class Column {
   public:
    std::string name;
    type::Type type;

    bool is_primary_key = false;

    Column();
    Column(const std::string& name, const type::Type& type);

    void set_primary_key(bool set);
};

class Table {
   public:
    uint64_t id;

    std::string name;
    std::vector<Column> columns;

    partition_t partition;

    Table();
    Table(const std::string& name, const std::vector<Column>& columns);
};

absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns);

absl::Status drop_table(const std::string& table_name);

// Set the partition strategy for a table, reset the partition strategy if
// the table already has one.
absl::Status set_partition(const std::string& table_name,
                           const std::string& partition_column,
                           PgQuery__PartitionStrategy strategy);

absl::Status add_list_partition(const std::string& table_name,
                                const std::string& partition_name,
                                const std::vector<std::string>& values);

absl::Status add_partition_constraint(
    const std::string& partition_name,
    const std::pair<std::string, std::string>& constraint);

}  // namespace schema
