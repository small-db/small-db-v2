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
// self header
// =====================================================================

#include "src/server_base/args.h"

namespace small::server_base {
// Static instance pointer definition (must be outside class)
ServerInfo* ServerInfo::instance = nullptr;

ServerArgs::ServerArgs()
    : sql_port(5432),
      grpc_port(50051),
      region("default"),
      join(""),
      data_dir("./data/default") {}

ServerArgs::ServerArgs(int sql_port, int grpc_port, std::string region,
                       std::string join, std::string data_dir)
    : sql_port(sql_port),
      grpc_port(grpc_port),
      region(std::move(region)),
      join(std::move(join)),
      data_dir(std::move(data_dir)) {}

ServerInfo::ServerInfo() = default;
ServerInfo::~ServerInfo() = default;

void ServerInfo::Init(const ServerArgs& args) {
    if (instance == nullptr) {
        instance = new ServerInfo();
        instance->db_path = args.data_dir;
    }
}

absl::StatusOr<ServerInfo*> ServerInfo::GetInstance() {
    if (!instance) {
        return absl::InternalError("ServerInfo instance is not initialized");
    }
    return instance;
}

// Now this definition is in only one place
absl::StatusOr<ServerInfo*> get_info() { return ServerInfo::GetInstance(); }

}  // namespace small::server_base
