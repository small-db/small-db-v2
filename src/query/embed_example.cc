#include <iostream>

#include "battery/embed.h"

namespace query {
void print_embed() {
    std::cout << "file content: \n"
              << b::embed<"resources/banner.txt">() << std::endl;
}
}  // namespace query