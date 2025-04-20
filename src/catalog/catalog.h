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

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// =====================================================================
// local libraries
// =====================================================================

#include "src/rocks/rocks.h"
#include "src/schema/schema.h"

namespace small::catalog {

class Catalog {
   private:
    // singleton instance - the only instance
    static Catalog* instancePtr;

    // singleton instance - constructor protector
    Catalog();

    // singleton instance - destructor protector
    ~Catalog() = default;

    std::unordered_map<std::string, std::shared_ptr<small::schema::Table>>
        tables;
    std::shared_ptr<small::schema::Table> system_tables;
    std::shared_ptr<small::schema::Table> system_partitions;

    std::optional<std::shared_ptr<small::schema::Table>> get_table(
        const std::string& table_name);

    small::rocks::RocksDBWrapper* db;

   public:
    // singleton instance - assignment-blocker
    void operator=(const Catalog&) = delete;

    // singleton instance - copy-blocker
    Catalog(const Catalog&) = delete;

    // singleton instance - get api
    static Catalog* GetInstance();

    // singleton instance - init api
    static void InitInstance();

    absl::Status create_table(
        const std::string& table_name,
        const std::vector<small::schema::Column>& columns);
};

}  // namespace small::catalog
