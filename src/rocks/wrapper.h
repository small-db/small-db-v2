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

#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include <rocksdb/db.h>
#include <rocksdb/options.h>

namespace rocks_wrapper {

class RocksDBWrapper {
   public:
    RocksDBWrapper(const std::string& db_path,
                   const std::vector<std::string>& column_family_names);
    ~RocksDBWrapper();

    bool Put(const std::string& cf_name, const std::string& key,
             const std::string& value);
    bool Get(const std::string& cf_name, const std::string& key,
             std::string& value);
    bool Delete(const std::string& cf_name, const std::string& key);

    void PrintAllKV();

   private:
    rocksdb::DB* db_;
    std::unordered_map<std::string, rocksdb::ColumnFamilyHandle*> cf_handles_;

    void Close();
    rocksdb::ColumnFamilyHandle* GetColumnFamilyHandle(
        const std::string& cf_name);
};

}  // namespace rocks_wrapper