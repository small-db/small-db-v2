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
#include "absl/strings/str_format.h"

// =====================================================================
// local libraries
// =====================================================================

#include "src/encode/encode.h"
#include "src/rocks/rocks.h"
#include "src/schema/schema.h"
#include "src/type/type.h"

// ====================================================================
// self header
// ====================================================================

#include "src/kv_store/kv_store.h"

namespace small::kv_store {

void write_row(small::rocks::RocksDBWrapper* db,
               const std::shared_ptr<small::schema::Table>& table,
               const std::vector<small::type::Datum>& values) {
    int pk_index = -1;
    for (int i = 0; i < table->columns.size(); ++i) {
        if (table->columns[i].is_primary_key) {
            pk_index = i;
            break;
        }
    }

    for (int i = 0; i < table->columns.size(); ++i) {
        auto key = absl::StrFormat("/%s/%s/column_%d", table->name,
                                   small::encode::encode(values[pk_index]), i);
        db->Put(key, small::encode::encode(values[i]));
    }
}

}  // namespace small::kv_store
