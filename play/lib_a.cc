#include <iostream>

#include "lib_b.h"

namespace lib_a {
    void lib_a_function() {
        std::cout << "lib_a_function called" << std::endl;
        lib_b::lib_b_self();
    }

    void lib_a_self() {
        std::cout << "lib_a_self called" << std::endl;
    }
}