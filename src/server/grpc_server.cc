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

#include <iostream>
#include <memory>
#include <string>

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

#include "hello.grpc.pb.h"
#include "hello.pb.h"

// =====================================================================
// self header
// =====================================================================

#include "src/server/grpc_server.h"

namespace small::grpc_server {

class FuckService final : public helloworld::Greeter::Service {
   public:
    virtual ::grpc::Status SayHello(::grpc::ServerContext* context,
                                    const ::helloworld::HelloRequest* request,
                                    ::helloworld::HelloReply* response) {
        std::cout << "Server: SayHello for \"" << request->name() << "\"."
                  << std::endl;
        response->set_message("Hello " + request->name() + "!");

        return grpc::Status::OK;
    }
};

void start_server(int port) {
    grpc::ServerBuilder builder;
    std::string addr = fmt::format("0.0.0.0:{}", port);
    builder.AddListeningPort(addr, grpc::InsecureServerCredentials());

    small::grpc_server::FuckService my_service;
    builder.RegisterService(&my_service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());

    // Run the server in a separate thread
    std::thread server_thread([&server]() { server->Wait(); });

    // Detach the thread to allow it to run independently
    server_thread.detach();
}

}  // namespace small::grpc_server
