#include "pipeline.hpp"
#include "model.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace VulkanEngine {
    Pipeline::Pipeline(
        Device& device,
        const std::string& vertPath, 
        const std::string& fragPath,
        const PipelineConfig& config) : device{device} {
        create(vertPath, fragPath, config);
    };

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(device.device(), vertShaderModule, nullptr);
        vkDestroyShaderModule(device.device(), fragShaderModule, nullptr);
        vkDestroyPipeline(device.device(), pipeline, nullptr);
    }
        
    std::vector<char> Pipeline::readFile(const std::string& path) {
        std::ifstream file(path, std::ios::ate | std::ios::binary);

        if (!file.is_open()) { throw std::runtime_error("failed to open file: " + path); }

        size_t size = file.tellg(); std::vector<char> buffer(size);
        
        file.seekg(0); file.read(buffer.data(), size);

        file.close(); return buffer;
    };

    void Pipeline::bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    };

    void Pipeline::create(
        const std::string& vertPath, 
        const std::string& fragPath,
        const PipelineConfig& config) {
        auto vertCode = readFile(vertPath);
        auto fragCode = readFile(fragPath);

        createShaderModule(vertCode, &vertShaderModule);
        createShaderModule(fragCode, &fragShaderModule);
        
        VkPipelineShaderStageCreateInfo stages[2];
        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].pSpecializationInfo = nullptr;
        stages[0].module = vertShaderModule;
        stages[0].pNext = nullptr;
        stages[0].pName = "main";
        stages[0].flags = 0;

        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[1].pSpecializationInfo = nullptr;
        stages[1].module = fragShaderModule;
        stages[1].pNext = nullptr;
        stages[1].pName = "main";
        stages[1].flags = 0;

        auto bindingDescriptions = Model::Vertex::getBindingDescripitons();
        auto attributeDescriptions = Model::Vertex::getAttributeDescripitons();
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
        vertexInputInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

        VkPipelineViewportStateCreateInfo viewportInfo{};
        viewportInfo.scissorCount = 1;
        viewportInfo.viewportCount = 1;
        viewportInfo.pScissors = &config.scissor;
        viewportInfo.pViewports = &config.viewport;
        viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.pDynamicState = nullptr;
        pipelineInfo.subpass = config.subpass;
        pipelineInfo.layout = config.pipelineLayout;
        pipelineInfo.renderPass = config.renderPass;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pColorBlendState = &config.colorBlendInfo;
        pipelineInfo.pMultisampleState = &config.multisampleInfo;
        pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
        pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
        pipelineInfo.pRasterizationState = &config.rasterizationInfo;
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

        if (vkCreateGraphicsPipelines(
          device.device(), VK_NULL_HANDLE, 1, 
          &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
              throw std::runtime_error("failed to create graphics pipeline");
        }
    };

    void Pipeline::createShaderModule(
        const std::vector<char>& code, 
        VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.codeSize = code.size();
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(
            device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        };
    }
    PipelineConfig Pipeline::defaultPipelineConfig(uint32_t width, uint32_t height) {
            PipelineConfig config{};

            config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

            config.viewport.x = 0.0f;
            config.viewport.y = 0.0f;
            config.viewport.width = width;
            config.viewport.height = height;
            config.viewport.minDepth = 0.0f;
            config.viewport.maxDepth = 1.0f;
            
            config.scissor.offset = {0, 0};
            config.scissor.extent = {width, height};
            
            config.rasterizationInfo.lineWidth = 1.0f;
            config.rasterizationInfo.depthBiasClamp = 0.0f;
            config.rasterizationInfo.depthBiasEnable = VK_FALSE;
            config.rasterizationInfo.depthClampEnable = VK_FALSE;
            config.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     
            config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
            config.rasterizationInfo.depthBiasConstantFactor = 0.0f;
            config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
            config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
            config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
            config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            
            config.multisampleInfo.pSampleMask = nullptr;
            config.multisampleInfo.minSampleShading = 1.0f;
            config.multisampleInfo.alphaToOneEnable = VK_FALSE;
            config.multisampleInfo.sampleShadingEnable = VK_FALSE;
            config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
            config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

            config.colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | 
                VK_COLOR_COMPONENT_G_BIT | 
                VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
                
            config.colorBlendAttachment.blendEnable = VK_FALSE;
            config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
            config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
            config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
            config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
            config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

            config.colorBlendInfo.attachmentCount = 1;
            config.colorBlendInfo.blendConstants[0] = 0.0f;  
            config.colorBlendInfo.blendConstants[1] = 0.0f;  
            config.colorBlendInfo.blendConstants[2] = 0.0f;  
            config.colorBlendInfo.blendConstants[3] = 0.0f;
            config.colorBlendInfo.logicOpEnable = VK_FALSE;
            config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
            config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
            config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

            config.depthStencilInfo.back = {};
            config.depthStencilInfo.front = {};  
            config.depthStencilInfo.minDepthBounds = 0.0f;  
            config.depthStencilInfo.maxDepthBounds = 1.0f; 
            config.depthStencilInfo.depthTestEnable = VK_TRUE;
            config.depthStencilInfo.depthWriteEnable = VK_TRUE;
            config.depthStencilInfo.stencilTestEnable = VK_FALSE;
            config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
            config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
            config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;   

            return config;
        }
}