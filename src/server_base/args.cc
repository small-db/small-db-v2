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
#include <utility>

// =====================================================================
// third-party libraries
// =====================================================================

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server_base/args.h"

namespace small::server_base {
// Static instance pointer definition (must be outside class)
ServerInfo* ServerInfo::instance = nullptr;

ServerArgs::ServerArgs(const std::string& sql_addr,
                       const std::string& grpc_addr,
                       const std::string& data_dir, const std::string& region,
                       const std::string& join)
    : sql_addr(sql_addr),
      grpc_addr(grpc_addr),
      data_dir(data_dir),
      region(region),
      join(join) {}

ServerInfo::ServerInfo() = default;
ServerInfo::~ServerInfo() = default;

absl::Status ServerInfo::Init(const ServerArgs& args) {
    if (instance == nullptr) {
        instance = new ServerInfo();
        instance->db_path = args.data_dir;
        return absl::OkStatus();
    }
    SPDLOG_ERROR("ServerInfo instance is already initialized");
    return absl::InternalError("ServerInfo instance is already initialized");
}

absl::StatusOr<ServerInfo*> ServerInfo::GetInstance() {
    if (!instance) {
        return absl::InternalError("ServerInfo instance is not initialized");
    }
    return instance;
}

absl::Status init(const ServerArgs& args) { return ServerInfo::Init(args); }

absl::StatusOr<ServerInfo*> get_info() { return ServerInfo::GetInstance(); }

}  // namespace small::server_base
