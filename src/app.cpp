#include "app.hpp"

#include <array>
#include <iostream>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace VulkanEngine {

    struct PushConstantData {
        glm::vec2 resolution;
        alignas(16)glm::vec3 data;
    };

    void App::run() {
        while (window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(device.device());
    };

    App::App() {
        loadModel();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    };

    App::~App() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    };

    void App::loadModel() {
        std::vector<Model::Vertex> vertices {
            {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{-1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
        };
        model = std::make_unique<Model>(device, vertices);
    };

    void App::createPipelineLayout() {

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | 
                                       VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);  

        VkPipelineLayoutCreateInfo pipelineLayoutInfo;
        pipelineLayoutInfo.flags = 0;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pNext = nullptr;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        
        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, 
                                   nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }; 

    void App::createPipeline() {
        auto pipelineConfig = Pipeline::defaultPipelineConfig(
            swapChain.width(), swapChain.height());
        pipelineConfig.renderPass = swapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device,
            "C:/Games/VulkanEngine/src/shaders/vert.spv",
            "C:/Games/VulkanEngine/src/shaders/frag.spv",
            pipelineConfig
        );
    };

    void App::createCommandBuffers() {
        commandBuffers.resize(swapChain.imageCount());

        VkCommandBufferAllocateInfo bufferInfo{};

        bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        bufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        bufferInfo.commandPool = device.getCommandPool();
        bufferInfo.commandBufferCount = commandBuffers.size();

        if (vkAllocateCommandBuffers(device.device(), &bufferInfo, 
                                     commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers");                             
        }
    };

    void App::updateCommandBuffers(int imageIndex) {
        static int frame = 0;
        frame = frame + 1;

        VkCommandBufferBeginInfo bufferInfo{};

        bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &bufferInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain.getRenderPass();
        renderPassInfo.framebuffer = swapChain.getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = clearValues.size();
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        pipeline->bind(commandBuffers[imageIndex]);

        model->bind(commandBuffers[imageIndex]);

        for (int j = 0; j < 1; j++) {
            double x, y; glfwGetCursorPos((GLFWwindow*)window.getWindow(), &x, &y);
            x = 2 * x / WIDTH - 1; y = 1 - 2 * y / HEIGHT;
            PushConstantData push{{WIDTH, HEIGHT}, {frame, -(float)y, -(float)x}};
            //push.data = {WIDTH, HEIGHT, frame};

            vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, 
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0, sizeof(PushConstantData), &push);

            model->draw(commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer");
        }
    }

    void App::freeCommandBuffers() {
        vkFreeCommandBuffers(
            device.device(),
            device.getCommandPool(),
            commandBuffers.size(),
            commandBuffers.data());
        commandBuffers.clear();
    };

    void App::drawFrame() {
        uint32_t imageIndex;
        auto result = swapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image");
        }

        updateCommandBuffers(imageIndex);

        result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) { throw std::runtime_error("failed to acquire swap chain image"); }
    };

    void onMouseMove(GLFWwindow* window, double x, double y) {
    }
}