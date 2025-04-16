#include <iostream>

#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>
#include <hello.grpc.pb.h>
#include <hello.pb.h>

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

int main(int argc, char* argv[]) {
    grpc::ServerBuilder builder;
    builder.AddListeningPort("0.0.0.0:50051",
                             grpc::InsecureServerCredentials());

    FuckService my_service;
    builder.RegisterService(&my_service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();

    return 0;
}
