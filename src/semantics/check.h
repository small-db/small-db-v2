

#pragma once

#include <absl/base/options.h>

#include <optional>
#include <string>

namespace semantics {

std::optional<std::string> is_string(PgQuery__Node* node);

}  // namespace semantics