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
#include "src/rocks/wrapper.h"
#include "src/schema/const.h"
#include "src/schema/schema.h"

namespace query {

std::tuple<std::string_view, std::string_view> parse_key(
    const std::string& key) {
    size_t first_slash = key.find('/');
    if (first_slash == std::string::npos) {
        throw std::invalid_argument("Invalid key format: missing first slash");
    }

    size_t second_slash = key.find('/', first_slash + 1);
    if (second_slash == std::string::npos) {
        throw std::invalid_argument("Invalid key format: missing second slash");
    }

    size_t third_slash = key.find('/', second_slash + 1);
    if (third_slash == std::string::npos) {
        throw std::invalid_argument("Invalid key format: missing third slash");
    }

    std::string_view table_name = std::string_view(key).substr(
        first_slash + 1, second_slash - first_slash - 1);
    std::string_view pk = std::string_view(key).substr(
        second_slash + 1, third_slash - second_slash - 1);

    return {table_name, pk};
}

std::shared_ptr<arrow::Schema> get_input_schema(const schema::Table& table) {
    arrow::FieldVector fields;
    for (const auto& column : table.columns) {
        fields.push_back(
            arrow::field(column.name, type::to_gandiva_type(column.type)));
    }
    return arrow::schema(fields);
}

arrow::Status query2(PgQuery__SelectStmt* select_stmt) {
    SPDLOG_ERROR("query");

    auto schemaname = select_stmt->from_clause[0]->range_var->schemaname;
    auto relname = select_stmt->from_clause[0]->range_var->relname;

    auto table_name = std::string(schemaname) + "." + std::string(relname);

    std::string db_path = schema::DATA_DIR;
    auto db = rocks_wrapper::RocksDBWrapper::GetInstance(db_path, {});
    auto scan_preix = "/" + table_name + "/";
    auto kv_pairs = db->GetAll(scan_preix);

    for (const auto& kv : kv_pairs) {
        SPDLOG_INFO("key: {}, value: {}", kv.first, kv.second);

        auto [table_name, pk] = parse_key(kv.first);
        SPDLOG_INFO("table_name: {}, pk: {}", table_name, pk);
    }

    auto table = schema::get_table(table_name);
    if (!table) {
        SPDLOG_ERROR("table not found: {}", table_name);
        return arrow::Status::Invalid("table not found");
    }
    auto schema = get_input_schema(*table.value());

    SPDLOG_INFO("schema: {}", schema->ToString());

    return arrow::Status::OK();
}

arrow::Status query3(PgQuery__SelectStmt* select_stmt) {
    // std::shared_ptr<arrow::Field> field_table_name =
    //     arrow::field("table_name", arrow::utf8());
    // std::shared_ptr<arrow::Field> field_columns =
    //     arrow::field("columns", arrow::utf8());
    // std::shared_ptr<arrow::Schema> input_schema =
    //     arrow::schema({field_table_name, field_columns});

    // auto in_batch =
    //     arrow::RecordBatch::Make(input_schema, num_records, {array});

    return arrow::Status::OK();

    // arrow::StringBuilder pk_builder;

    // ARROW_ASSIGN_OR_RAISE(std::shared_ptr<arrow::Array> array,
    //                       pk_builder.Finish());

    // std::shared_ptr<arrow::Field> field_table_name =
    //     arrow::field("table_name", arrow::utf8());
    // std::shared_ptr<arrow::Field> field_columns =
    //     arrow::field("columns", arrow::utf8());
    // std::shared_ptr<arrow::Schema> input_schema =
    //     arrow::schema({field_table_name, field_columns});

    // int num_records = kv_pairs.size();

    // auto in_batch =
    //     arrow::RecordBatch::Make(input_schema, num_records, {array});

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

    return arrow::Status::OK();
}

void query(PgQuery__SelectStmt* select_stmt) {
    auto status = query2(select_stmt);
    if (!status.ok()) {
        SPDLOG_ERROR("query failed: {}", status.ToString());
    }
}

}  // namespace query
