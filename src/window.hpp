#ifndef WINDOW_HPP
#define WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace VulkanEngine {
    class Window {
        private:
            GLFWwindow* window;

            int width, height;

            std::string name;

            void init();
        public:
            Window(const int width, const int height, std::string name); ~Window();

            bool shouldClose() { return !glfwWindowShouldClose(window); }

            VkExtent2D getExtent() { return {static_cast<uint32_t>(width), 
                                             static_cast<uint32_t>(height)}; };

            void createSurface(VkInstance instance, VkSurfaceKHR* surface);

            Window(const Window&) = delete;
            void operator=(const Window&) = delete;

            const GLFWwindow* getWindow() { return window;};

    };
}

#endif // WINDOW_HPP