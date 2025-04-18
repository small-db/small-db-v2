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
// c++ std
// =====================================================================

#include <string>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/statusor.h"

namespace small::server_base {
class ServerArgs {
   public:
    int sql_port;
    int grpc_port;

    std::string region;

    std::string join;

    std::string data_dir;

    ServerArgs();

    ServerArgs(int sql_port, int grpc_port, std::string region,
               std::string join = "", std::string data_dir = "");
};

class ServerInfo {
   private:
    // singleton instance - constructor protector
    ServerInfo();
    // singleton instance - destructor protector
    ~ServerInfo();

   public:
    // singleton instance - copy blocker
    ServerInfo(const ServerInfo&) = delete;

    // singleton instance - assignment blocker
    void operator=(const ServerInfo&) = delete;

    std::string db_path;

    static ServerInfo* instance;

    static void Init(const ServerArgs& args);

    // singleton instance - get instance
    static absl::StatusOr<ServerInfo*> GetInstance();
};

absl::StatusOr<ServerInfo*> get_info();

}  // namespace small::server_base
