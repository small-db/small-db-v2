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

// =====================================================================
// third-party libraries
// =====================================================================

// spdlog
#include "spdlog/spdlog.h"

// grpc
#include "grpc/grpc.h"
#include "grpcpp/server_builder.h"

// =====================================================================
// local libraries
// =====================================================================

#include "server_reg.grpc.pb.h"
#include "server_reg.pb.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server/grpc_server.h"

namespace server {

class AddressBookService final : public helloworld::Greeter::Service {
   public:
    // virtual ::grpc::Status GetAddress(::grpc::ServerContext* context,
    //                                   const ::helloworld::NameQuerry*
    //                                   request,
    //                                   ::expcmake::Address* response) {
    //     std::cout << "Server: GetAddress for \"" << request->name() << "\"."
    //               << std::endl;

    //     response->set_name("Peter Peterson");
    //     response->set_zip("12345");
    //     response->set_country("Superland");

    //     return grpc::Status::OK;
    // }
};

void RunGrpcServer(int port) {
    std::thread grpc_server_thread([port]() {
        SPDLOG_INFO("Starting gRPC server on port {}", port);

        grpc::ServerBuilder builder;
        std::string addr = fmt::format("0.0.0.0:{}", port);
        builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

        AddressBookService my_service;
        builder.RegisterService(&my_service);

        std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
        server->Wait();
    });
    grpc_server_thread.detach();
}

}  // namespace server
