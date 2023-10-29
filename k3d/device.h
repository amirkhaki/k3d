#pragma once

#include "Window.h"

#define VULKAN_HPP_NO_CONSTRUCTORS

#include <vulkan/vulkan.hpp>
// std lib headers
#include <string>
#include <vector>
#include <optional>

namespace k3d {

    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

    class Device {
    public:
#ifdef NDEBUG
        const bool enableValidationLayers = false;
#else
        const bool enableValidationLayers = true;
#endif

        explicit Device(Window &window);

        ~ Device();

        // Not copyable or movable
        Device(const Device &) = delete;

        void operator=(const Device &) = delete;

        Device(Device &&) = delete;

        Device &operator=(Device &&) = delete;

        vk::CommandPool getCommandPool() { return commandPool; }

        vk::Device device() { return device_.get(); }

        vk::SurfaceKHR surface() { return surface_; }

        vk::Queue graphicsQueue() { return graphicsQueue_; }

        vk::Queue presentQueue() { return presentQueue_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }

        uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags propertyFlags);

        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }

        vk::Format findSupportedFormat(
                const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);

        // Buffer Helper Functions
        void createBuffer(
                vk::DeviceSize size,
                vk::BufferUsageFlags usage,
                vk::MemoryPropertyFlags propertyFlags,
                vk::Buffer &buffer,
                vk::UniqueDeviceMemory &bufferMemory);

        vk::CommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(vk::CommandBuffer commandBuffer);

        void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);

        void copyBufferToImage(
                vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height, uint32_t layerCount);

        void createImageWithInfo(
                const vk::ImageCreateInfo &imageInfo,
                vk::MemoryPropertyFlags propertyFlags,
                vk::Image &image,
                vk::DeviceMemory &imageMemory);

        vk::PhysicalDeviceProperties properties;

    private:
        void createInstance();

        void setupDebugMessenger();

        void createSurface();

        void pickPhysicalDevice();

        void createLogicalDevice();

        void createCommandPool();

        // helper functions
        bool isDeviceSuitable(vk::PhysicalDevice device);

        [[nodiscard]] std::vector<const char *> getRequiredExtensions() const;

        bool checkValidationLayerSupport();

        QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device);

        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        void hasGflwRequiredInstanceExtensions();

        bool checkDeviceExtensionSupport(vk::PhysicalDevice device);

        SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device);

        vk::Instance instance;
        VkDebugUtilsMessengerEXT debugMessenger{};
        vk::PhysicalDevice physicalDevice;
        Window &window;
        vk::CommandPool commandPool;

        vk::UniqueDevice device_;
        vk::SurfaceKHR surface_;
        vk::Queue graphicsQueue_;
        vk::Queue presentQueue_;

        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };

}  // namespace lve