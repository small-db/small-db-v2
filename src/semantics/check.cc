
#include <absl/base/options.h>

namespace semantics {

absl::Option<string> is_string(PgQuery__Node* node) {
    if (node->node_case == PG_QUERY__NODE__NODE_STRING) {
        return absl::Some(node->string);
    } else {
        return absl::None;
    }
}

}  // namespace semantics