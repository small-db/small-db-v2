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

namespace small::server_registry {

class Server {};

class ServerRegister {
   private:
    // singleton instance - constructor protector
    ServerRegister();
    // singleton instance - destructor protector
    ~ServerRegister();

   public:
    // singleton instance - copy blocker
    ServerRegister(const ServerRegister&) = delete;

    // singleton instance - assignment blocker
    void operator=(const ServerRegister&) = delete;

    // singleton instance - get instance
    static ServerRegister* GetInstance() {
        static ServerRegister instance;
        return &instance;
    }
};

void start_server(int port);

// get servers according to the constraints, pass an empty constraints to get
// all servers
std::vector<std::shared_ptr<Server>> get_servers(
    std::unordered_map<std::string, std::string>& constraints);

absl::Status join(std::string peer_addr);

}  // namespace small::server_registry
