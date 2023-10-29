//
// Created by amir on 10/23/23.
//

#include <iostream>
#include "App.h"
#include <ranges>

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
        model = loadModels();
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
            vk::ArrayWrapper1D<float, 4> colors({0.0, 0.0, 0.0, 1.0});
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
            model->bind(cmd.get());
            model->draw(cmd.get());
            cmd->endRenderPass();
            try {
                cmd->end();
            } catch (const std::exception &) {
                throw std::runtime_error("failed to end command buffer");
            }
        }
        return commandBuffersV;
    }

    namespace {
        struct triangle {
            glm::vec2 v1, v2, v3;
        };

        glm::vec2 operator/(glm::vec2 v, int n) {
            return {v.x / n, v.y / n};;
        }

        static void algorithm(triangle t, std::vector<triangle> &tv, int depth, int n = 0) {
            if (n >= depth) {
                tv.push_back(t);
                return;
            }
            auto m12 = (t.v1 + t.v2) / 2;
            auto m13 = (t.v1 + t.v3) / 2;
            auto m23 = (t.v2 + t.v3) / 2;
            ++n;
            algorithm({t.v1, m12, m13}, tv, depth, n);
            algorithm({t.v2, m12, m23}, tv, depth, n);
            algorithm({t.v3, m23, m13}, tv, depth, n);
        }

        std::vector<Model::Vertex> sierpinski(glm::vec2 v1, glm::vec2 v2, glm::vec2 v3, int depth) {
            std::vector<triangle> triangles;
            std::vector<Model::Vertex> r;
            algorithm({v1, v2, v3}, triangles, depth, 0);
            for (auto &i: std::views::reverse(triangles)) {
                r.push_back({i.v1});
                r.push_back({i.v2});
                r.push_back({i.v3});
            }
            return r;
        }

    }

    std::unique_ptr<Model> App::loadModels() {
        std::vector<Model::Vertex> vertices = sierpinski(
                {1, 0.9},
                {0.0f, -1.0f},
                {-1.0f, 0.9f},
                10
        );
        return std::make_unique<Model>(device, vertices);
    }

} // k3d