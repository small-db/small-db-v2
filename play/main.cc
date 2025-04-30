#include "absl/status/status.h"
#include <iostream>

#include "lib_a.h"
#include "lib_b.h"

int main() {
    absl::Status status = absl::InternalError("Something went wrong!");
    if (!status.ok()) std::cerr << "Error: " << status.message() << std::endl;
    return 0;
}