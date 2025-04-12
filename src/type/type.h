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

// =====================================================================
// third-party libraries
// =====================================================================

// absl
#include "absl/status/statusor.h"

// // arrow gandiva
#include "gandiva/arrow.h"

namespace type {

enum class Type {
    Int64 = 10,
    String = 20,
};

using Datum = std::variant<int64_t, std::string>;

absl::StatusOr<Type> from_string(const std::string& type_name);

std::string to_string(Type type);

gandiva::DataTypePtr to_gandiva_type(Type type);

}  // namespace type
