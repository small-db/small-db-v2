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

#include "src/server_base/args.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server_registry/server_registry.h"

namespace small::server_registry {

absl::Status ServerRegister::RegisterServer(
    const small::server_base::ServerArgs& args) {
    SPDLOG_INFO("register server: sql_address: {}, rpc_address: {}, region: {}",
                args.sql_addr, args.grpc_addr, args.region);
    this->servers.push_back(args);
    return absl::OkStatus();
}

ServerRegister* ServerRegister::GetInstance() {
    return instance;
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

        auto status = small::server_registry::ServerRegister::GetInstance()
                          ->RegisterServer(small::server_base::ServerArgs(
                              request->sql_address(), request->rpc_address(),
                              request->region(), "", ""));

        if (!status.ok()) {
            SPDLOG_ERROR("failed to register server: {}", status.ToString());
            response->set_success(false);
            return grpc::Status(grpc::StatusCode::INTERNAL, status.ToString());
        }

        response->set_success(true);

        return grpc::Status::OK;
    }
};

std::vector<small::server_base::ServerArgs> get_servers(
    std::unordered_map<std::string, std::string>& constraints) {
    std::vector<small::server_base::ServerArgs> result;
    auto servers =
        small::server_registry::ServerRegister::GetInstance()->servers;
    SPDLOG_INFO("get servers: {}", servers.size());
    for (const auto& server : servers) {
        SPDLOG_INFO("server: sql_address: {}, rpc_address: {}, region: {}",
                    server.sql_addr, server.grpc_addr, server.region);
        SPDLOG_INFO("constraints: ");
        for (const auto& [k, v] : constraints) {
            SPDLOG_INFO("key: {}, value: {}", k, v);
        }
        bool match = true;
        for (const auto& [k, v] : constraints) {
            if (k == "sql_address" && server.sql_addr != v) {
                match = false;
                break;
            } else if (k == "rpc_address" && server.grpc_addr != v) {
                match = false;
                break;
            } else if (k == "region" && server.region != v) {
                match = false;
                break;
            }
        }
        if (match) {
            result.push_back(server);
        }
    }
    return result;
}

void start_server(std::string addr) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

    auto service = std::make_shared<small::server_registry::RegistryService>();
    builder.RegisterService(service.get());

    auto server = builder.BuildAndStart();
    std::thread([server = std::move(server), service]() mutable {
        server->Wait();
    }).detach();
}

absl::Status join(const small::server_base::ServerArgs& args) {
    if (args.join.empty()) {
        return absl::OkStatus();
    }

    SPDLOG_INFO("join peer addr: {}", args.join);

    small::server_registry::RegistryRequest request;
    request.set_sql_address(args.sql_addr);
    request.set_rpc_address(args.grpc_addr);
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
