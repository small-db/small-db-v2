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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/status.h"

// grpc
#include "grpcpp/server_builder.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/server_base/args.h"

namespace small::server_registry {

class ServerRegister {
   private:
    // singleton instance - constructor protector
    ServerRegister();
    // singleton instance - destructor protector
    ~ServerRegister();

    inline static ServerRegister* instance;

    std::mutex mutex_;

   public:
    // singleton instance - copy blocker
    ServerRegister(const ServerRegister&) = delete;

    // singleton instance - assignment blocker
    void operator=(const ServerRegister&) = delete;

    // singleton instance - get instance
    static ServerRegister* GetInstance();

    std::vector<small::server_base::ServerArgs> servers;

    absl::Status RegisterServer(const small::server_base::ServerArgs& args);
};

void start_server(std::string addr);

// get servers according to the constraints, pass an empty constraints to get
// all servers
std::vector<small::server_base::ServerArgs> get_servers(
    std::unordered_map<std::string, std::string>& constraints);

// absl::Status join(const small::server_base::ServerArgs& args);
absl::Status join(small::server_base::ServerArgs args);

}  // namespace small::server_registry
