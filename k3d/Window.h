//
// Created by amir on 10/23/23.
//

#ifndef K3D_WINDOW_H
#define K3D_WINDOW_H

#define VULKAN_HPP_NO_CONSTRUCTORS

#include <GLFW/glfw3.h>

#include <string>
#include <vulkan/vulkan.hpp>

namespace k3d {

    class Window {
    public:
        Window(int w, int h, std::string name);

        ~Window();

        Window(const Window &) = delete;

        Window operator=(const Window &) = delete;


        bool shouldClose();

        void createWindowSurface(vk::Instance instance, vk::SurfaceKHR &pKhr);

        [[nodiscard]] vk::Extent2D getExtent() const { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

    private:
        void initWindow();

        std::string windowName;
        int height;
        int width;
        GLFWwindow *window;
    };


} // k3d

#endif //K3D_WINDOW_H
