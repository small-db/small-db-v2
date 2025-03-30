

#pragma once

#include <optional>
#include <string>

#include <absl/base/options.h>

namespace semantics {

std::optional<std::string> is_string(PgQuery__Node* node);

std::string node_type_str(PgQuery__Node* node);

}  // namespace semantics