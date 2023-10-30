//
// Created by amir on 10/23/23.
//

#ifndef K3D_APP_H
#define K3D_APP_H

#include "Window.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Model.h"
#include <memory>

namespace k3d {

    class App {
    public:
        App();

        static constexpr int HEIGHT = 600;
        static constexpr int WIDTH = 800;

        void run();

    private:
        vk::UniquePipelineLayout createPipelineLayout();

        std::unique_ptr<Pipeline> createPipeline();

        std::vector<vk::UniqueCommandBuffer> createCommandBuffers();

        std::unique_ptr<Model> loadModels();

        void drawFrame();

        void recordCommandBuffer(vk::CommandBuffer cmd, uint32_t imageIndex);

        Window window{WIDTH, HEIGHT, "first app"};
        Device device{window};
        std::unique_ptr<SwapChain> swapchain;
        std::unique_ptr<Pipeline> pipeline;
        vk::UniquePipelineLayout pipelineLayout;
        std::vector<vk::UniqueCommandBuffer> commandBuffers;
        std::unique_ptr<Model> model;

        std::unique_ptr<SwapChain> createSwapChain();

        void recreateSwapChain();
    };

} // k3d

#endif //K3D_APP_H
