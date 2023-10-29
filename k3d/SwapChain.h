#pragma once

#include "device.h"

// vulkan headers
#include <vulkan/vulkan.hpp>

// std lib headers
#include <string>
#include <vector>

namespace k3d {

    class SwapChain {
    public:
        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(Device &deviceRef, vk::Extent2D windowExtent);

        ~SwapChain();

        SwapChain(const SwapChain &) = delete;

        void operator=(const SwapChain &) = delete;

        vk::Framebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index].get(); }

        vk::RenderPass getRenderPass() { return renderPass.get(); }

        vk::ImageView getImageView(int index) { return swapChainImageViews[index]; }

        size_t imageCount() { return swapChainImages.size(); }

        vk::Format getSwapChainImageFormat() { return swapChainImageFormat; }

        vk::Extent2D getSwapChainExtent() { return swapChainExtent; }

        [[nodiscard]] uint32_t width() const { return swapChainExtent.width; }

        [[nodiscard]] uint32_t height() const { return swapChainExtent.height; }

        [[nodiscard]] float extentAspectRatio() const {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }

        vk::Format findDepthFormat();

        vk::Result acquireNextImage(uint32_t &imageIndex);

        vk::Result submitCommandBuffers(const vk::CommandBuffer *buffers, uint32_t &imageIndex);

    private:
        void createSwapChain();

        void createImageViews();

        void createDepthResources();

        void createRenderPass();

        void createFramebuffers();

        void createSyncObjects();

        // Helper functions
        static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(
                const std::vector<vk::SurfaceFormatKHR> &availableFormats);

        static vk::PresentModeKHR chooseSwapPresentMode(
                const std::vector<vk::PresentModeKHR> &availablePresentModes);

        vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities);

        vk::Format swapChainImageFormat;
        vk::Extent2D swapChainExtent;

        std::vector<vk::UniqueFramebuffer> swapChainFramebuffers;
        vk::UniqueRenderPass renderPass;

        std::vector<vk::Image> depthImages;
        std::vector<vk::DeviceMemory> depthImageMemories;
        std::vector<vk::ImageView> depthImageViews;
        std::vector<vk::Image> swapChainImages;
        std::vector<vk::ImageView> swapChainImageViews;

        Device &device;
        vk::Extent2D windowExtent;

        vk::UniqueSwapchainKHR swapChain;

        std::vector<vk::Semaphore> imageAvailableSemaphores;
        std::vector<vk::Semaphore> renderFinishedSemaphores;
        std::vector<vk::Fence> inFlightFences;
        std::vector<vk::Fence> imagesInFlight;
        size_t currentFrame = 0;
    };

}  // namespace k3d
