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
void query(PgQuery__SelectStmt* select_stmt) {
    SPDLOG_ERROR("query");
    // std::shared_ptr<arrow::RecordBatch> result =
    //     arrow::RecordBatch::Make(output_schema, outputs[0]->length(),
    //     outputs);
    // //(Doc section: Evaluate projection)

    // std::cout << "Project result:" << std::endl;
    // std::cout << result->ToString() << std::endl;
}

}  // namespace query
