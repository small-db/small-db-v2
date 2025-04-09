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

#include <string>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// magic_enum
#include "magic_enum/magic_enum.hpp"

// pg_query
#include "pg_query.h"
#include "pg_query.pb-c.h"

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/schema/schema.h"
#include "src/semantics/check.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server/stmt_handler.h"

namespace stmt_handler {

absl::Status handle_create_table(PgQuery__CreateStmt* create_stmt) {
    std::string table_name = create_stmt->relation->relname;
    std::vector<schema::Column> columns;

    for (int i = 0; i < create_stmt->n_table_elts; i++) {
        auto node_case = create_stmt->table_elts[i]->node_case;
        switch (node_case) {
            case PG_QUERY__NODE__NODE_COLUMN_DEF: {
                auto column_def = create_stmt->table_elts[i]->column_def;

                // choose the last name as the type name
                // Q: why?
                // A:
                // int -> [pg_catalog, int4]
                // double -> [pg_catalog, float8]
                // string -> [string]
                int name_id = column_def->type_name->n_names - 1;

                auto type_name =
                    semantics::is_string(column_def->type_name->names[name_id]);

                bool primary_key = false;
                for (int j = 0; j < column_def->n_constraints; j++) {
                    auto constraint = column_def->constraints[j]->constraint;
                    switch (constraint->contype) {
                        case PG_QUERY__CONSTR_TYPE__CONSTR_PRIMARY:
                            primary_key = true;
                            SPDLOG_INFO("constraint->contype: {}",
                                        static_cast<int>(constraint->contype));
                            break;

                        default:
                            break;
                    }
                }

                schema::Column column(column_def->colname, type_name.value());
                if (primary_key) {
                    column.set_primary_key(true);
                }
                columns.push_back(column);

                break;
            }
            case PG_QUERY__NODE__NODE_CONSTRAINT: {
                SPDLOG_ERROR("constraint");
                break;
            }
            default:
                SPDLOG_ERROR("unknown table element, node_case: {}",
                             static_cast<int>(node_case));
                break;
        }
    }

    auto status = schema::create_table(table_name, columns);
    if (!status.ok()) {
        SPDLOG_ERROR("create table failed: {}", status.ToString());
        return status;
    }

    if (create_stmt->partspec != NULL) {
        auto strategy = create_stmt->partspec->strategy;
        if (create_stmt->partspec->n_part_params != 1) {
            SPDLOG_ERROR("number of part params: {}",
                         create_stmt->partspec->n_part_params);
            return absl::OkStatus();
        }

        auto partition_column = std::string(
            create_stmt->partspec->part_params[0]->partition_elem->name);

        auto status =
            schema::set_partition(table_name, partition_column, strategy);
        if (!status.ok()) {
            SPDLOG_ERROR("set partitioning failed: {}", status.ToString());
            return status;
        }
    }

    return absl::OkStatus();
}

absl::Status handle_drop_table(PgQuery__DropStmt* drop_stmt) {
    auto table_name = drop_stmt->objects[0]->list->items[0]->string->sval;
    return schema::drop_table(table_name);
}

absl::Status handle_add_partition(PgQuery__CreateStmt* create_stmt) {
    auto table_name = create_stmt->inh_relations[0]->range_var->relname;
    auto partition_name = create_stmt->relation->relname;

    std::vector<std::string> values;
    for (int i = 0; i < create_stmt->partbound->n_listdatums; i++) {
        const auto& datum = create_stmt->partbound->listdatums[i];
        values.push_back(datum->a_const->sval->sval);
    }

    return schema::add_list_partition(table_name, partition_name, values);
}

absl::Status handle_stmt(PgQuery__Node* stmt) {
    switch (stmt->node_case) {
        case PG_QUERY__NODE__NODE_CREATE_STMT: {
            auto create_stmt = stmt->create_stmt;
            if (create_stmt->n_inh_relations == 0) {
                return handle_create_table(create_stmt);
            } else {
                return handle_add_partition(create_stmt);
            }
            break;
        }
        case PG_QUERY__NODE__NODE_DROP_STMT: {
            return handle_drop_table(stmt->drop_stmt);
            break;
        }
        case PG_QUERY__NODE__NODE_TRANSACTION_STMT: {
            SPDLOG_INFO("transaction statement");
            break;
        }
        case PG_QUERY__NODE__NODE_ALTER_TABLE_STMT: {
            auto subtype =
                stmt->alter_table_stmt->cmds[0]->alter_table_cmd->subtype;
            SPDLOG_INFO("subtype: {}",
                        std::string(magic_enum::enum_name(subtype)));
            SPDLOG_ERROR("alter table statement");

            auto partition_name = stmt->alter_table_stmt->relation->relname;
            auto expr =
                stmt->alter_table_stmt->cmds[0]
                    ->alter_table_cmd->def->constraint->raw_expr->a_expr;
            auto lexpr = expr->lexpr->column_ref->fields[0]->string->sval;
            auto op = expr->name[0]->string->sval;
            auto rexpr = expr->rexpr->a_const->sval->sval;
            SPDLOG_INFO("partition_name: {}, lexpr: {}, op: {}, rexpr: {}",
                        partition_name, lexpr, op, rexpr);
            break;
        }
        case PG_QUERY__NODE__NODE_INSERT_STMT: {
            SPDLOG_ERROR("insert statement");
            break;
        }
        default:
            SPDLOG_ERROR("unknown statement, node_case: {}",
                         static_cast<int>(stmt->node_case));
            return absl::InvalidArgumentError(
                fmt::format("unknown statement, node_case: {}",
                            static_cast<int>(stmt->node_case)));
            break;
    }

    return absl::OkStatus();
}

}  // namespace stmt_handler
