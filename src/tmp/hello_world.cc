#include <iostream>
#include <thread>

int main() {
    std::cout << "Hello, World!" << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(20));

    std::cout << "Hello, World!" << std::endl;

    return 0;
}
