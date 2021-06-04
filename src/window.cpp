#include "window.hpp"

#include <stdexcept>

namespace VulkanEngine {
    Window::Window(const int width, const int height, std::string name) {
        this->name = name;
        this->width = width;
        this->height = height;
        init();
    }
    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    void Window::init() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
    }
    void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface)) {
            throw std::runtime_error("failed to create window surface");
        }
    }
}


