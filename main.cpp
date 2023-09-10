#include <iostream>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_funcs.hpp>

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	auto *window = glfwCreateWindow(800, 600, "k3d", nullptr, nullptr);
	uint32_t ext_count = 0;
	auto result = vk::enumerateInstanceExtensionProperties(nullptr, &ext_count, nullptr);
	std::cout << ext_count << " extensions and result is " << result << std::endl;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
