#include "src/schema/schema.h"

#include <absl/status/status.h>

namespace schema {
absl::Status create_table(const std::string& table_name,
                          const std::vector<Column>& columns) {
    return absl::OkStatus();
}
}  // namespace schema