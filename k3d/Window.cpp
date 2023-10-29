//
// Created by amir on 10/23/23.
//

#define GLFW_INCLUDE_VULKAN

#include "Window.h"

#include <utility>

namespace k3d {
    Window::Window(int w, int h, std::string name) : width(w), height(h), windowName(std::move(name)), window(nullptr) {
        initWindow();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    Window::~Window() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void Window::createWindowSurface(vk::Instance instance, vk::SurfaceKHR &pKhr) {
        VkSurfaceKHR rawSurface;
        if (glfwCreateWindowSurface(VkInstance(instance), window, nullptr, &rawSurface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        pKhr = rawSurface;
    }
} // k3d