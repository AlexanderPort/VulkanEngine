#ifndef MODEL_HPP
#define MODEL_HPP

#include "device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace VulkanEngine {
    class Model {
        public:
            struct Vertex {
                glm::vec2 position; glm::vec3 color;
                static std::vector<VkVertexInputBindingDescription> getBindingDescripitons();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescripitons();
            };
            
            Model(Device &device, const std::vector<Vertex> &vertices); ~Model();

            Model(const Model&) = delete;
            void operator=(const Model&) = delete;

            void bind(VkCommandBuffer commandBuffer);

            void draw(VkCommandBuffer commandBuffer);

        private:
            Device& device;
            uint32_t vertexCount;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            void createVertexBuffers(const std::vector<Vertex> &vertices);
    };
}

#endif // MODEL_HPP