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

#include <string>

namespace server {
class ServerArgs {
   public:
    int sql_port;

    std::string region;

    std::string join;

    std::string data_dir;

    ServerArgs()
        : sql_port(5432),
          region("default"),
          join(""),
          data_dir("./data/default") {}

    ServerArgs(int port, std::string region, std::string join = "",
               std::string data_dir = "")
        : sql_port(port), region(region), join(join), data_dir(data_dir) {}
};

// extern ServerArgs DefaultArgs;

}  // namespace server
