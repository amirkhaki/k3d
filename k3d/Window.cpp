//
// Created by amir on 10/23/23.
//

#define GLFW_INCLUDE_VULKAN

#include "Window.h"

#include <utility>
#include <iostream>

namespace k3d {
    Window::Window(int w, int h, std::string name) : width(w), height(h), windowName(std::move(name)), window(nullptr) {
        initWindow();
    }

    void Window::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
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

    void Window::framebufferResizedCallback(GLFWwindow *window, int width, int height) {
        auto windowClass = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        windowClass->width = width;
        windowClass->height = height;
        windowClass->framebufferResized = true;
    }
} // k3d