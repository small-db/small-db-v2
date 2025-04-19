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
// self header
// =====================================================================

#include "src/encode/encode.h"

namespace encode {

std::string encode(const small::typeDatum& datum) {
    if (std::holds_alternative<int64_t>(datum)) {
        return std::to_string(std::get<int64_t>(datum));
    } else if (std::holds_alternative<std::string>(datum)) {
        return std::get<std::string>(datum);
    }
    throw std::runtime_error("Unsupported type for encoding");
}

small::typeDatum decode(const std::string& str, small::typeType type) {
    switch (type) {
        case small::typeType::Int64:
            return std::stoll(str);
        case small::typeType::String:
            return str;
        default:
            throw std::runtime_error("Unsupported type for decoding");
    }
}

}  // namespace encode
