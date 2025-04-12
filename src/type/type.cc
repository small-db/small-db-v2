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

// arrow
#include "arrow/api.h"

// arrow gandiva
#include "gandiva/arrow.h"

// =====================================================================
// self include
// =====================================================================

#include "src/type/type.h"

namespace type {

absl::StatusOr<Type> from_string(const std::string& type_name) {
    if (type_name == "int4") {
        return Type::Int64;
    } else if (type_name == "string") {
        return Type::String;
    } else {
        return absl::InvalidArgumentError("unknown type: " + type_name);
    }
}

std::string to_string(Type type) {
    switch (type) {
        case Type::Int64:
            return "int4";
        case Type::String:
            return "string";
        default:
            throw std::runtime_error("unknown type");
    }
}

gandiva::DataTypePtr to_gandiva_type(Type type) {
    switch (type) {
        case Type::Int64:
            return arrow::int64();
        case Type::String:
            return arrow::utf8();
        default:
            throw std::runtime_error("Unsupported type for Gandiva");
    }
}

}  // namespace type
