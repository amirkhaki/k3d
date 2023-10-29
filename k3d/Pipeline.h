//
// Created by amir on 10/23/23.
//

#ifndef K3D_PIPELINE_H
#define K3D_PIPELINE_H

#include <string>
#include <vector>
#include "device.h"

namespace k3d {
    struct PipelineConfigInfo {
        vk::Viewport viewport;
        vk::Rect2D scissor;
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        vk::PipelineRasterizationStateCreateInfo rasterizationInfo;
        vk::PipelineMultisampleStateCreateInfo multisampleInfo;
        vk::PipelineColorBlendAttachmentState colorBlendAttachment;
        vk::PipelineColorBlendStateCreateInfo colorBlendInfo;
        vk::PipelineDepthStencilStateCreateInfo depthStencilInfo;
        vk::PipelineLayout pipelineLayout;
        vk::RenderPass renderPass;
        uint32_t subpass = 0;
    };


    class Pipeline {
    public:
        Pipeline(Device &device,
                 const std::string &vertFilePath,
                 const std::string &fragFilePath,
                 const PipelineConfigInfo &configInfo);

        ~ Pipeline();

        Pipeline(const Pipeline &) = delete;

        Pipeline operator=(const Pipeline &) = delete;

        void bind(vk::CommandBuffer commandBuffer);

        static PipelineConfigInfo defaultConfig(uint32_t width, uint32_t height);

    private:
        static std::vector<char> readFile(const std::string &filePath);

        vk::UniquePipeline createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath,
                                                  const PipelineConfigInfo &configInfo);

        vk::UniqueShaderModule createShaderModule(const std::vector<char> &code);


        Device &device;
        vk::UniquePipeline pipeline;
        vk::UniqueShaderModule vertShaderModule;
        vk::UniqueShaderModule fragShaderModule;
    };

} // k3d

#endif //K3D_PIPELINE_H
