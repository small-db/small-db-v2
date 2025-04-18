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
#include <utility>
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

// =====================================================================
// local libraries
// =====================================================================

#include "src/server/args.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server_registry/server_registry.h"

namespace small::server_registry {

absl::Status ServerRegister::RegisterServer(const server::ServerArgs& args) {
    SPDLOG_INFO("register server: sql_address: {}, rpc_address: {}, region: {}",
                args.sql_port, args.grpc_port, args.region);
    return absl::OkStatus();
}

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

    auto service = std::make_shared<small::server_registry::RegistryService>();
    builder.RegisterService(service.get());

    auto server = builder.BuildAndStart();
    std::thread([server = std::move(server), service]() mutable {
        server->Wait();  // blocks forever
    }).detach();
}

absl::Status join(const server::ServerArgs& args) {
    if (args.join.empty()) {
        return absl::OkStatus();
    }

    SPDLOG_INFO("join peer addr: {}", args.join);

    small::server_registry::RegistryRequest request;
    request.set_sql_address(std::to_string(args.sql_port));
    request.set_rpc_address(std::to_string(args.grpc_port));
    request.set_region(args.region);

    auto channel =
        grpc::CreateChannel(args.join, grpc::InsecureChannelCredentials());
    std::unique_ptr<small::server_registry::ServerRegistry::Stub> stub =
        small::server_registry::ServerRegistry::NewStub(channel);
    grpc::ClientContext context;
    small::server_registry::RegistryReply result;
    grpc::Status status = stub->Register(&context, request, &result);
    if (!status.ok()) {
        SPDLOG_ERROR("failed to join peer: {}", status.error_message());
        return absl::InternalError(status.error_message());
    }
    SPDLOG_INFO("joined peer: {}", result.success());
    return absl::OkStatus();
}

}  // namespace small::server_registry
