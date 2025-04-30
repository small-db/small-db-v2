#pragma once

#include <iostream>

#include "lib_a.h"

namespace lib_b {
    void lib_b_function() {
        std::cout << "lib_b_function called" << std::endl;
        lib_a::lib_a_self();
    }

    void lib_b_self() {
        std::cout << "lib_b_self called" << std::endl;
    }
}