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

#pragma once

// =====================================================================
// third-party libraries
// =====================================================================

// pg_query
#include "pg_query.h"
#include "pg_query.pb-c.h"

// absl
#include "absl/status/statusor.h"

// arrow
#include "arrow/api.h"

// =====================================================================
// protobuf generated files
// =====================================================================

#include "insert.grpc.pb.h"
#include "insert.pb.h"

namespace insert {

absl::StatusOr<std::shared_ptr<arrow::RecordBatch>> insert(
    PgQuery__InsertStmt* insert_stmt);

class InsertService final : public ::small::insert::InsertService::Service {
   public:
    virtual ::grpc::Status Insert(::grpc::ServerContext* context,
                                  const ::small::insert::Row* request,
                                  ::small::insert::InsertReply* response);
};

}  // namespace insert
