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
// third-party libraries
// =====================================================================

// grpc
#include "grpc/grpc.h"
#include "grpcpp/server_builder.h"

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// protobuf generated files
// =====================================================================

#include "server_registry.grpc.pb.h"
#include "server_registry.pb.h"
// #include "server_registry.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server_registry/server_registry.h"

namespace small::server_registry {

class ServerRegService final
    : public small::server_registry::ServerRegistry::Service {
   public:
    virtual ::grpc::Status Register(
        ::grpc::ServerContext* context,
        const ::small::server_registry::RegistryRequest* request,
        ::small::server_registry::RegistryReply* response) {}
};

std::vector<std::shared_ptr<Server>> get_servers(
    std::unordered_map<std::string, std::string>& constraints) {
    return std::vector<std::shared_ptr<Server>>();
}

void start_server(int port) {
    grpc::ServerBuilder builder;
    std::string addr = fmt::format("0.0.0.0:{}", port);
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

    ServerRegService service;
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::thread server_thread([&server]() { server->Wait(); });
    server_thread.detach();
}

absl::Status join(std::string peer_addr) {
    SPDLOG_INFO("join peer addr: {}", peer_addr);
    return absl::OkStatus();
}

}  // namespace small::server_registry
