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

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// =====================================================================
// protobuf generated files
// =====================================================================

#include "server_reg.grpc.pb.h"
#include "server_reg.pb.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server_reg/server_reg.h"

namespace small::server_reg {

class ServerRegService final : public small::server_reg::ServerReg::Service {
   public:
    virtual ::grpc::Status Register(
        ::grpc::ServerContext* context,
        const ::small::server_reg::ServerRegRequest* request,
        ::small::server_reg::ServerRegReply* response) {}
};

std::vector<std::shared_ptr<Server>> get_servers(
    std::unordered_map<std::string, std::string>& constraints) {
    return std::vector<std::shared_ptr<Server>>();
}

}  // namespace small::server_reg
