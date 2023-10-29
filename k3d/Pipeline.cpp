//
// Created by amir on 10/23/23.
//

#include "Pipeline.h"
#include "Model.h"

#include <fstream>
#include <iostream>

namespace k3d {
    Pipeline::Pipeline(Device &device, const std::string &vertFilePath, const std::string &fragFilePath,
                       const PipelineConfigInfo &configInfo) : device(device) {
        pipeline = createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
    }

    std::vector<char> Pipeline::readFile(const std::string &filePath) {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("failed to open file: " + filePath);
        }
        std::streamsize fileSize = file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();
        return buffer;
    }

    vk::UniquePipeline
    Pipeline::createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath,
                                     const PipelineConfigInfo &configInfo) {
        auto vert = readFile(vertFilePath);
        auto frag = readFile(fragFilePath);
        vertShaderModule = createShaderModule(vert);
        fragShaderModule = createShaderModule(frag);

        vk::PipelineShaderStageCreateInfo stageCreateInfos[2]{
                {
                        .stage = vk::ShaderStageFlagBits::eVertex,
                        .module = vertShaderModule.get(),
                        .pName = "main",
                },
                {
                        .stage = vk::ShaderStageFlagBits::eFragment,
                        .module = fragShaderModule.get(),
                        .pName = "main",
                }
        };

        auto vertexBindingDescriptions = Model::Vertex::getBindingDescriptions();
        auto vertexAttributeDescriptions = Model::Vertex::getAttributeDescriptions();
        vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{
                .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size()),
                .pVertexBindingDescriptions = vertexBindingDescriptions.data(),
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size()),
                .pVertexAttributeDescriptions = vertexAttributeDescriptions.data(),
        };
        vk::PipelineViewportStateCreateInfo viewportInfo{
                .viewportCount = 1,
                .pViewports = &configInfo.viewport,
                .scissorCount = 1,
                .pScissors = &configInfo.scissor,
        };

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{
                .stageCount = 2,
                .pStages = stageCreateInfos,
                .pVertexInputState = &vertexInputStateCreateInfo,
                .pInputAssemblyState = &configInfo.inputAssemblyInfo,
                .pViewportState = &viewportInfo,
                .pRasterizationState = &configInfo.rasterizationInfo,
                .pMultisampleState = &configInfo.multisampleInfo,
                .pDepthStencilState = &configInfo.depthStencilInfo,
                .pColorBlendState = &configInfo.colorBlendInfo,
                .layout = configInfo.pipelineLayout,
                .renderPass = configInfo.renderPass,
                .subpass = configInfo.subpass,

                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = -1,
        };

        try {
            return device.device().createGraphicsPipelineUnique(vk::PipelineCache(), pipelineCreateInfo).value;
        } catch (const std::exception &e) {
            throw std::runtime_error("failed to create graphics pipeline");
        }

    }

    vk::UniqueShaderModule Pipeline::createShaderModule(const std::vector<char> &code) {
        vk::ShaderModuleCreateInfo createInfo{
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const uint32_t *>(code.data()),
        };
        try {
            return device.device().createShaderModuleUnique(createInfo);
        } catch (const std::exception &e) {
            throw std::runtime_error("unable to create shader module");
        }
    }

    Pipeline::~Pipeline() =
    default;

    PipelineConfigInfo Pipeline::defaultConfig(uint32_t width, uint32_t height) {
        PipelineConfigInfo configInfo{
                .viewport = {
                        .x = 0.0f,
                        .y = 0.0f,
                        .width = static_cast<float>(width),
                        .height = static_cast<float>(height),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f,
                },

                .scissor = {
                        .offset = {0, 0},
                        .extent = {width, height}
                },

                .inputAssemblyInfo = {
                        .topology = vk::PrimitiveTopology::eTriangleList,
                        .primitiveRestartEnable =  false,
                },

                .rasterizationInfo = {
                        .depthClampEnable = false,
                        .rasterizerDiscardEnable = false,
                        .polygonMode = vk::PolygonMode::eFill,
                        .cullMode = vk::CullModeFlagBits::eNone,
                        .frontFace = vk::FrontFace::eClockwise,
                        .depthBiasEnable =  false,
                        .depthBiasConstantFactor = 0.0f,  // Optional
                        .depthBiasClamp = 0.0f,           // Optional
                        .depthBiasSlopeFactor = 0.0f,     // Optional
                        .lineWidth = 1.0f,
                },

                .multisampleInfo = {
                        .rasterizationSamples = vk::SampleCountFlagBits::e1,
                        .sampleShadingEnable =  false,
                        .minSampleShading = 1.0f,           // Optional
                        .pSampleMask = nullptr,           // Optional
                        .alphaToCoverageEnable =  false,  // Optional
                        .alphaToOneEnable =  false,       // Optional
                },

                .colorBlendAttachment = {
                        .blendEnable =  false,
                        .srcColorBlendFactor = vk::BlendFactor::eOne,   // Optional
                        .dstColorBlendFactor = vk::BlendFactor::eZero,  // Optional
                        .colorBlendOp = vk::BlendOp::eAdd,              // Optional
                        .srcAlphaBlendFactor = vk::BlendFactor::eOne,   // Optional
                        .dstAlphaBlendFactor = vk::BlendFactor::eZero,  // Optional
                        .alphaBlendOp = vk::BlendOp::eAdd,              // Optional
                        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                          vk::ColorComponentFlagBits::eB |
                                          vk::ColorComponentFlagBits::eA,
                },

                .colorBlendInfo = {
                        .logicOpEnable =  false,
                        .logicOp = vk::LogicOp::eCopy,  // Optional
                        .attachmentCount = 1,
                        .pAttachments = &configInfo.colorBlendAttachment,
                        .blendConstants = {{0.0f, 0.0, 0.0, 0.0}},  // Optional
                },

                .depthStencilInfo = {
                        .depthTestEnable = true,
                        .depthWriteEnable = true,
                        .depthCompareOp = vk::CompareOp::eLess,
                        .depthBoundsTestEnable =  false,
                        .stencilTestEnable =  false,
                        .front = {},  // Optional
                        .back = {},   // Optional
                        .minDepthBounds = 0.0f,  // Optional
                        .maxDepthBounds = 1.0f,  // Optional
                },
        };

        return configInfo;
    }

    void Pipeline::bind(vk::CommandBuffer commandBuffer) {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());
    }

} // k3d