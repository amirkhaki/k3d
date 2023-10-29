//
// Created by amir on 10/23/23.
//

#include <iostream>
#include "App.h"

namespace k3d {
    void App::run() {
        while (!window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }
        device.device().waitIdle();
    }

    vk::UniquePipelineLayout App::createPipelineLayout() {
        vk::PipelineLayoutCreateInfo layout{
                .setLayoutCount = 0,
                .pSetLayouts = nullptr,
                .pushConstantRangeCount = 0,
                .pPushConstantRanges = nullptr,
        };
        try {
            return device.device().createPipelineLayoutUnique(layout);
        } catch (const std::exception &) {
            throw std::runtime_error("failed to create pipeline layout");
        }
    }

    std::unique_ptr<Pipeline> App::createPipeline() {
        auto pipelineConfig = Pipeline::defaultConfig(swapchain.width(), swapchain.height());
        pipelineConfig.renderPass = swapchain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout.get();
        return std::make_unique<Pipeline>(device,
                                          "shaders/triangle.vert.spv",
                                          "shaders/triangle.frag.spv",
                                          pipelineConfig);
    }

    App::App() {
        pipelineLayout = createPipelineLayout();
        pipeline = createPipeline();
        commandBuffers = createCommandBuffers();
    }

    void App::drawFrame() {
        uint32_t imageIndex;
        vk::resultCheck(swapchain.acquireNextImage(imageIndex), "failed to acquire next image");
        vk::resultCheck(swapchain.submitCommandBuffers(&commandBuffers[imageIndex].get(), imageIndex),
                        "failed to submit command buffer");

    }

    std::vector<vk::UniqueCommandBuffer> App::createCommandBuffers() {
        vk::CommandBufferAllocateInfo allocateInfo{
                .commandPool = device.getCommandPool(),
                .level = vk::CommandBufferLevel::ePrimary,
                .commandBufferCount = static_cast<uint32_t>(swapchain.imageCount()),
        };

        std::vector<vk::UniqueCommandBuffer> commandBuffersV;
        try {
            commandBuffersV = device.device().allocateCommandBuffersUnique(allocateInfo);
        } catch (const std::exception &) {
            throw std::runtime_error("failed to allocate command buffers");
        }
        for (int i = 0; i < commandBuffersV.size(); ++i) {
            auto &cmd = commandBuffersV[i];
            vk::CommandBufferBeginInfo beginInfo{};
            try {
                cmd->begin(beginInfo);
            } catch (const std::exception &) {
                throw std::runtime_error("failed to begin recording command buffer");
            }
            vk::ArrayWrapper1D<float, 4> colors({0.9, 0.9, 0.9, 0.9});
            std::array<vk::ClearValue, 2> clearValues{};
            clearValues[0].color = vk::ClearColorValue{
                    .float32 = colors //vk::ArrayWrapper1D<float, 4>({0.1, 0.1, 0.1f, 1.0f})
            };
            clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0, 0};
            vk::RenderPassBeginInfo renderPassBeginInfo{
                    .renderPass = swapchain.getRenderPass(),
                    .framebuffer = swapchain.getFrameBuffer(i),
                    .renderArea = {.offset = {0, 0}, .extent = swapchain.getSwapChainExtent(),},
                    .clearValueCount = clearValues.size(),
                    .pClearValues = clearValues.data(),
            };
            cmd->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
            pipeline->bind(cmd.get());
            cmd->draw(3, 1, 0, 0);
            cmd->endRenderPass();
            try {
                cmd->end();
            } catch (const std::exception &) {
                throw std::runtime_error("failed to end command buffer");
            }
        }
        return commandBuffersV;
    }
} // k3d