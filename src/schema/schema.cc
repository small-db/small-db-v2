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

// absl
#include "absl/status/status.h"
#include "absl/strings/str_format.h"

// json
#include "nlohmann/json.hpp"

// rocksdb
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"

// spdlog
#include "spdlog/spdlog.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/encode/encode.h"
#include "src/id/generator.h"
#include "src/insert/insert.h"
#include "src/rocks/rocks.h"
#include "src/schema/const.h"
#include "src/schema/partition.h"
#include "src/server_base/args.h"
#include "src/type/type.h"

// =====================================================================
// self header
// =====================================================================

#include "src/schema/schema.h"

namespace small::schema {

void to_json(nlohmann::json& j, const Column& c) {
    j = nlohmann::json{
        {"name", c.name},
        {"type", c.type},
        {"is_primary_key", c.is_primary_key},
    };
}

void from_json(const nlohmann::json& j, Column& c) {
    j.at("name").get_to(c.name);
    j.at("type").get_to(c.type);
    j.at("is_primary_key").get_to(c.is_primary_key);
}

void to_json(nlohmann::json& j, const Table& t) {
    j = nlohmann::json{{"name", t.name}, {"columns", t.columns}};
}

void from_json(const nlohmann::json& j, Table& t) {
    j.at("name").get_to(t.name);
    j.at("columns").get_to(t.columns);
}

}  // namespace small::schema
