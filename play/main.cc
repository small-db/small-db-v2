#include "absl/status/status.h"
#include <iostream>

int main() {
    absl::Status status = absl::InternalError("Something went wrong!");
    if (!status.ok()) std::cerr << "Error: " << status.message() << std::endl;
    return 0;
}