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

#include "src/rocks/wrapper.h"

namespace rocks_wrapper {

RocksDBWrapper::RocksDBWrapper(
    const std::string& db_path,
    const std::vector<std::string>& column_family_names) {
    rocksdb::Options options;
    options.create_if_missing = true;
    options.create_missing_column_families = true;

    std::vector<rocksdb::ColumnFamilyDescriptor> cf_descriptors;
    std::vector<rocksdb::ColumnFamilyHandle*> handles;

    // Always add the default column family
    cf_descriptors.emplace_back(rocksdb::kDefaultColumnFamilyName,
                                rocksdb::ColumnFamilyOptions());

    // Add user-defined column families
    for (const auto& name : column_family_names) {
        cf_descriptors.emplace_back(name, rocksdb::ColumnFamilyOptions());
    }

    // Open database with column families
    rocksdb::Status status =
        rocksdb::DB::Open(options, db_path, cf_descriptors, &handles, &db_);
    if (!status.ok()) {
        throw std::runtime_error("Failed to open RocksDB: " +
                                 status.ToString());
    }

    // Map column family handles to names
    for (size_t i = 0; i < cf_descriptors.size(); ++i) {
        cf_handles_[cf_descriptors[i].name] = handles[i];
    }
}

RocksDBWrapper::~RocksDBWrapper() { Close(); }

void RocksDBWrapper::Close() {
    for (const auto& cf : cf_handles_) {
        db_->DestroyColumnFamilyHandle(cf.second);
    }
    delete db_;
}

rocksdb::ColumnFamilyHandle* RocksDBWrapper::GetColumnFamilyHandle(
    const std::string& cf_name) {
    auto it = cf_handles_.find(cf_name);
    if (it == cf_handles_.end()) {
        throw std::runtime_error("Column Family not found: " + cf_name);
    }
    return it->second;
}

bool RocksDBWrapper::Put(const std::string& cf_name, const std::string& key,
                         const std::string& value) {
    auto* handle = GetColumnFamilyHandle(cf_name);
    rocksdb::Status status =
        db_->Put(rocksdb::WriteOptions(), handle, key, value);
    return status.ok();
}

bool RocksDBWrapper::Get(const std::string& cf_name, const std::string& key,
                         std::string& value) {
    auto* handle = GetColumnFamilyHandle(cf_name);
    rocksdb::Status status =
        db_->Get(rocksdb::ReadOptions(), handle, key, &value);
    return status.ok();
}

std::vector<std::pair<std::string, std::string>> RocksDBWrapper::GetAllKV(
    const std::string& cf_name) {
    std::vector<std::pair<std::string, std::string>> kv_pairs;

    // Get the column family handle
    auto* handle = GetColumnFamilyHandle(cf_name);

    // Create an iterator for the column family
    rocksdb::ReadOptions read_options;
    std::unique_ptr<rocksdb::Iterator> it(
        db_->NewIterator(read_options, handle));

    // Iterate through all key-value pairs
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        kv_pairs.emplace_back(it->key().ToString(), it->value().ToString());
    }

    // Check for any errors during iteration
    if (!it->status().ok()) {
        throw std::runtime_error("Error during iteration: " +
                                 it->status().ToString());
    }

    return kv_pairs;
}

bool RocksDBWrapper::Delete(const std::string& cf_name,
                            const std::string& key) {
    auto* handle = GetColumnFamilyHandle(cf_name);
    rocksdb::Status status = db_->Delete(rocksdb::WriteOptions(), handle, key);
    return status.ok();
}

void RocksDBWrapper::PrintAllKV() {
    for (const auto& cf : cf_handles_) {
        std::cout << "Column Family: " << cf.first << std::endl;

        rocksdb::ReadOptions read_options;
        rocksdb::Iterator* it = db_->NewIterator(read_options, cf.second);
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            std::cout << "\tKey: " << it->key().ToString()
                      << ", Value: " << it->value().ToString() << std::endl;
        }
        delete it;
    }
}

}  // namespace rocks_wrapper