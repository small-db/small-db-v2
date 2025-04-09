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
#include <unordered_map>
#include <vector>

namespace schema {

class NullPartition {};

void to_json(nlohmann::json& j, const NullPartition& p);

void from_json(const nlohmann::json& j, NullPartition& p);

class ListPartition {
   public:
    // the partition column
    std::string column_name;

    // key: partition name
    // value: the values of the partition
    std::unordered_map<std::string, std::vector<std::string>> values;

    // key: partition name
    // value: the constraints of the partition
    //      e.g. (key) region = (value) 'asia'
    std::unordered_map<std::string,
                       std::unordered_map<std::string, std::string>>
        constraints;
};

void to_json(nlohmann::json& j, const ListPartition& p);

void from_json(const nlohmann::json& j, ListPartition& p);

using partition_t = std::variant<ListPartition>;

}  // namespace schema
