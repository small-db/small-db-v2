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

// CLI11
#include <CLI/CLI.hpp>

// =====================================================================
// local libraries
// =====================================================================

#include "src/server/server.h"

int main(int argc, char *argv[]) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%@] %v");

    CLI::App app{"small-db"};

    int sql_port = 0;
    app.add_option("--sql-port", sql_port, "SQL port number")
        ->required()
        ->check(CLI::Range(0, 65535));

    int grpc_port = 0;
    app.add_option("--grpc-port", grpc_port, "gRPC port number")
        ->required()
        ->check(CLI::Range(0, 65535));

    std::string region;
    app.add_option("--region", region, "Region name");

    std::string join;
    app.add_option("--join", join, "Join server address");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    return server::RunServer(
        small::server_base::ServerArgs{sql_port, grpc_port, region, join});
}
