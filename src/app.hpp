#ifndef APP_HPP
#define APP_HPP

#include "model.hpp"
#include "window.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

#include <vector>
#include <memory>

namespace VulkanEngine {
    class App {
        private:
            Window window{WIDTH, HEIGHT, "Vulkan"};
            Device device{window};
            SwapChain swapChain{device, window.getExtent()};

            std::unique_ptr<Pipeline> pipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<Model> model;

            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void updateCommandBuffers(int imageIndex);
            void freeCommandBuffers();
            void drawFrame();
            void loadModel();

            /*
            Pipeline pipeline{
                device,
                "C:/Games/VulkanEngine/src/shaders/vert.spv", 
                "C:/Games/VulkanEngine/src/shaders/frag.spv",
                Pipeline::defaultPipelineConfig(WIDTH, HEIGHT)};
            */
        public:
            static constexpr int WIDTH = 2000;
            static constexpr int HEIGHT = 1500;

            App(); ~App();

            App(const App&) = delete;
            void operator=(const App&) = delete;

            void run();

            void onMouseMove(GLFWwindow* window, double x, double y);
    };
}

#endif // APP_HPP