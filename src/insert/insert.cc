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

// =====================================================================
// c++ std
// =====================================================================

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// pg_query
#include "pg_query.h"
#include "pg_query.pb-c.h"

// spdlog
#include "spdlog/spdlog.h"

// arrow
#include "arrow/api.h"
#include "arrow/compute/api_vector.h"
#include "arrow/status.h"

// arrow gandiva
#include "gandiva/filter.h"
#include "gandiva/projector.h"
#include "gandiva/selection_vector.h"
#include "gandiva/tree_expr_builder.h"

// magic_enum
#include "magic_enum/magic_enum.hpp"

// =====================================================================
// self header
// =====================================================================

#include "src/insert/insert.h"
#include "src/schema/schema.h"

namespace insert {

absl::StatusOr<std::shared_ptr<arrow::RecordBatch>> insert(
    PgQuery__InsertStmt* insert_stmt) {
    auto table_name = insert_stmt->relation->relname;
    auto result = schema::get_table(table_name);
    if (!result) {
        return absl::InvalidArgumentError(
            fmt::format("table {} not found", table_name));
    }

    auto table = result.value();
    if (auto* listP = std::get_if<schema::ListPartition>(&table->partition)) {
        auto partition_column = listP->column_name;

        // get partition column id (in the insert statement)
        int partition_column_id = -1;
        for (int i = 0; i < insert_stmt->n_cols; i++) {
            if (insert_stmt->cols[i]->res_target->name == partition_column) {
                partition_column_id = i;
                break;
            }
        }

        if (partition_column_id == -1) {
            return absl::InvalidArgumentError(
                fmt::format("partition column {} not found", partition_column));
        }

        // process row by row
        int row_count = insert_stmt->select_stmt->select_stmt->n_values_lists;
        for (int row_id = 0; row_id < row_count; row_id++) {
            // get the partition value
            auto row =
                insert_stmt->select_stmt->select_stmt->values_lists[row_id];
            auto partition_value =
                row->list->items[partition_column_id]->a_const->sval->sval;
            SPDLOG_INFO("partition value: {}", partition_value);

            // get the partition
            auto partition = listP->lookup(partition_value);
            if (!partition) {
                return absl::InvalidArgumentError(fmt::format(
                    "partition not found for value {}", partition_value));
            }

            // partition->constraints;
            SPDLOG_INFO("partition constraints: {}", partition->constraints);
        }
    }

    return absl::InvalidArgumentError("insert statement is not supported yet");
}

}  // namespace insert
