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

// =====================================================================
// self header
// =====================================================================

#include "src/query/query.h"

namespace query {
arrow::Status query2(PgQuery__SelectStmt* select_stmt) {
    SPDLOG_ERROR("query");

    return arrow::Status::OK();

    std::shared_ptr<arrow::Field> field_result =
        arrow::field("table_name", arrow::utf8());

    std::shared_ptr<arrow::Schema> output_schema =
        arrow::schema({field_result});

    std::shared_ptr<gandiva::Projector> projector;

    std::shared_ptr<arrow::Field> field_table_name =
        arrow::field("table_name", arrow::utf8());
    std::shared_ptr<arrow::Field> field_columns =
        arrow::field("columns", arrow::utf8());
    std::shared_ptr<arrow::Schema> input_schema =
        arrow::schema({field_table_name, field_columns});

    int num_records = 1;

    arrow::Int32Builder builder;
    int32_t values[4] = {1, 2, 3, 4};
    ARROW_RETURN_NOT_OK(builder.AppendValues(values, 4));
    ARROW_ASSIGN_OR_RAISE(std::shared_ptr<arrow::Array> array,
                          builder.Finish());

    auto in_batch =
        arrow::RecordBatch::Make(input_schema, num_records, {array});

    auto pool = arrow::default_memory_pool();

    arrow::ArrayVector outputs;
    arrow::Status status;
    status = projector->Evaluate(*in_batch, pool, &outputs);

    std::shared_ptr<arrow::RecordBatch> result =
        arrow::RecordBatch::Make(output_schema, outputs[0]->length(), outputs);

    SPDLOG_INFO("project result: {}", result->ToString());
}

void query(PgQuery__SelectStmt* select_stmt) {
    auto status = query2(select_stmt);
    if (!status.ok()) {
        SPDLOG_ERROR("query failed: {}", status.ToString());
    }
}

}  // namespace query
