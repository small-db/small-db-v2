
#include <absl/base/options.h>
#include <pg_query.h>
#include <pg_query.pb-c.h>

#include <optional>
#include <string>

namespace semantics {

std::optional<std::string> is_string(PgQuery__Node* node) {
    if (node->node_case == PG_QUERY__NODE__NODE_STRING) {
        return node->string->sval;
    } else {
        return std::nullopt;
    }
}

}  // namespace semantics