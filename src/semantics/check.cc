
#include <absl/base/options.h>
#include <pg_query.h>
#include <pg_query.pb-c.h>

#include <optional>
#include <string>

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

namespace semantics {

std::optional<std::string> is_string(PgQuery__Node* node) {
    if (node->node_case == PG_QUERY__NODE__NODE_STRING) {
        return node->string->sval;
    } else {
        return std::nullopt;
    }
}

std::string node_type_str(PgQuery__Node* node) {
    auto v = magic_enum::enum_name(node->node_case);
    SPDLOG_INFO("node_case: {}", v);
    return std::string(v);
}

}  // namespace semantics