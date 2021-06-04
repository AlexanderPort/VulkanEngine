
#include "app.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main () {
    VulkanEngine::App app{};

    try {
        app.run();
    } catch (const std::exception& exception) {
        std::cout << exception.what() << std::endl;
        std::cerr << exception.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}