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
#include "grpcpp/create_channel.h"
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

class RegistryService final
    : public small::server_registry::ServerRegistry::Service {
   public:
    virtual ::grpc::Status Register(
        ::grpc::ServerContext* context,
        const ::small::server_registry::RegistryRequest* request,
        ::small::server_registry::RegistryReply* response) {
        SPDLOG_INFO(
            "register server: sql_address: {}, rpc_address: {}, region: {}",
            request->sql_address(), request->rpc_address(), request->region());

        response->set_success(true);

        return grpc::Status::OK;
    }
};

std::vector<std::shared_ptr<Server>> get_servers(
    std::unordered_map<std::string, std::string>& constraints) {
    return std::vector<std::shared_ptr<Server>>();
}

void start_server(int port) {
    grpc::ServerBuilder builder;
    std::string addr = fmt::format("0.0.0.0:{}", port);
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

    RegistryService service;
    builder.RegisterService(&service);
    SPDLOG_INFO("grpc server listening on {}", addr);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::thread server_thread([&server]() { server->Wait(); });
    server_thread.detach();
}

absl::Status join(std::string peer_addr, std::string self_region) {
    if (peer_addr.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(100));
        return absl::OkStatus();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1000));

    SPDLOG_INFO("join peer addr: {}", peer_addr);

    small::server_registry::RegistryRequest request;
    request.set_sql_address("a");
    request.set_rpc_address("b");
    request.set_region("c");

    auto channel =
        grpc::CreateChannel(peer_addr, grpc::InsecureChannelCredentials());
    std::unique_ptr<small::server_registry::ServerRegistry::Stub> stub =
        small::server_registry::ServerRegistry::NewStub(channel);
    grpc::ClientContext context;
    small::server_registry::RegistryReply result;
    grpc::Status status = stub->Register(&context, request, &result);
    if (!status.ok()) {
        SPDLOG_ERROR("failed to join peer: {}, self: {}",
                     status.error_message(), self_region);
        return absl::InternalError(status.error_message());
    }
    SPDLOG_INFO("joined peer: {}", result.success());
    return absl::OkStatus();
}

}  // namespace small::server_registry
