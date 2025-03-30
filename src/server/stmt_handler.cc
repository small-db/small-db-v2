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

#include <pg_query.h>
#include <pg_query.pb-c.h>

#include <spdlog/spdlog.h>

#include "src/schema/schema.h"
#include "src/semantics/check.h"

namespace stmt_handler {

void handle_stmt(PgQuery__Node* stmt) {
    switch (stmt->node_case) {
        case PG_QUERY__NODE__NODE_CREATE_STMT: {
            SPDLOG_INFO("create statement");
            auto create_stmt = stmt->create_stmt;
            SPDLOG_INFO("create_stmt->relation->relname: {}",
                        create_stmt->relation->relname);

            std::string table_name = create_stmt->relation->relname;
            std::vector<schema::Column> columns;

            if (create_stmt->n_inh_relations > 0) {
                SPDLOG_INFO("create_stmt->n_inh_relations: {}",
                            create_stmt->n_inh_relations);
                return;
            }

            std::string partition_column = "";
            PgQuery__PartitionStrategy strategy =
                PG_QUERY__PARTITION_STRATEGY__PARTITION_STRATEGY_UNDEFINED;

            if (create_stmt->partspec != NULL) {
                strategy = create_stmt->partspec->strategy;
                if (create_stmt->partspec->n_part_params != 1) {
                    SPDLOG_ERROR("number of part params: {}",
                                 create_stmt->partspec->n_part_params);
                    return;
                }

                partition_column =
                    std::string(create_stmt->partspec->part_params[0]
                                    ->partition_elem->name);
            }

            for (int i = 0; i < create_stmt->n_table_elts; i++) {
                auto node_case = create_stmt->table_elts[i]->node_case;
                switch (node_case) {
                    case PG_QUERY__NODE__NODE_COLUMN_DEF: {
                        SPDLOG_INFO("column definition");
                        auto column_def =
                            create_stmt->table_elts[i]->column_def;
                        SPDLOG_INFO("column_def->colname: {}",
                                    column_def->colname);

                        SPDLOG_INFO("column_def->type_name->n_names: {}",
                                    column_def->type_name->n_names);

                        // choose the last name as the type name
                        // Q: why?
                        // A:
                        // int -> [pg_catalog, int4]
                        // double -> [pg_catalog, float8]
                        // string -> [string]
                        int name_id = column_def->type_name->n_names - 1;

                        auto type_name = semantics::is_string(
                            column_def->type_name->names[name_id]);
                        if (!type_name.has_value()) {
                            SPDLOG_ERROR("type_name: unknown");
                        } else {
                            SPDLOG_INFO("type_name: {}", type_name.value());
                        }

                        bool primary_key = false;
                        for (int j = 0; j < column_def->n_constraints; j++) {
                            auto constraint =
                                column_def->constraints[j]->constraint;
                            switch (constraint->contype) {
                                case PG_QUERY__CONSTR_TYPE__CONSTR_PRIMARY:
                                    primary_key = true;
                                    SPDLOG_INFO(
                                        "constraint->contype: {}",
                                        static_cast<int>(constraint->contype));
                                    break;

                                default:
                                    break;
                            }
                        }

                        schema::Column column(column_def->colname,
                                              type_name.value());
                        if (primary_key) {
                            column.set_primary_key(true);
                        }
                        if (column_def->colname == partition_column) {
                            column.set_partitioning(strategy);
                        }
                        columns.push_back(column);

                        break;
                    }
                    case PG_QUERY__NODE__NODE_CONSTRAINT: {
                        SPDLOG_INFO("constraint");
                        break;
                    }
                    default:
                        SPDLOG_INFO("unknown table element, node_case: {}",
                                    static_cast<int>(node_case));
                        break;
                }
            }

            auto status = schema::create_table(table_name, columns);
            if (!status.ok()) {
                SPDLOG_ERROR("error creating table: {}", status.message());
            } else {
                SPDLOG_INFO("table created successfully");
            }

            break;
        }
        case PG_QUERY__NODE__NODE_TRANSACTION_STMT: {
            SPDLOG_INFO("transaction statement");
            break;
        }
        default:
            SPDLOG_INFO("unknown statement, node_case: {}",
                        static_cast<int>(stmt->node_case));
            break;
    }
}

}  // namespace stmt_handler