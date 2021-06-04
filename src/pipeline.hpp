#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "device.hpp"

#include <string>
#include <vector>

namespace VulkanEngine {
    struct PipelineConfig {
        VkRect2D scissor;
        VkViewport viewport;
        uint32_t subpass = 0;
        VkRenderPass renderPass = nullptr;
        VkPipelineLayout pipelineLayout = nullptr;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
    };

    class Pipeline {
        public:
            Pipeline(
                Device& device, 
                const std::string& vertPath, 
                const std::string& fragPath,
                const PipelineConfig& config);
            ~Pipeline();

            Pipeline(const Pipeline&) = delete;
            void operator=(const Pipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);

            static PipelineConfig defaultPipelineConfig(uint32_t width, uint32_t height);

        private:
            static std::vector<char> readFile(const std::string& path);

            void create(
                const std::string& vertPath, 
                const std::string& fragPath,
                const PipelineConfig& config);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            Device& device;
            VkPipeline pipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}

#endif // PIPELINE_HPP