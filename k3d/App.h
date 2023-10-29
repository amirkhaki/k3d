//
// Created by amir on 10/23/23.
//

#ifndef K3D_APP_H
#define K3D_APP_H

#include "Window.h"
#include "Pipeline.h"
#include "SwapChain.h"
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

        void drawFrame();

        Window window{WIDTH, HEIGHT, "first app"};
        Device device{window};
        SwapChain swapchain{device, window.getExtent()};
//        Pipeline pipeline{device,
//                          "shaders/triangle.vert.spv",
//                          "shaders/triangle.frag.spv",
//                          Pipeline::defaultConfig(WIDTH, HEIGHT)};
        std::unique_ptr<Pipeline> pipeline;
        vk::UniquePipelineLayout pipelineLayout;
        std::vector<vk::UniqueCommandBuffer> commandBuffers;
    };

} // k3d

#endif //K3D_APP_H
