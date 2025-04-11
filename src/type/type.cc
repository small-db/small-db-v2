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
        return absl::InvalidArgumentError("Unknown type: " + type_name);
    }
}

}  // namespace type
