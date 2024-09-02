#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace NNuts {
	class NNWindow {
	public:
		NNWindow(int w, int h, std::string name);
		~NNWindow();

		NNWindow(const NNWindow&) = delete;
		NNWindow& operator = (const NNWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(m_Window); }
		VkExtent2D getExtent() { 
			return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; 
		}
		bool wasWindowResized() { return m_FramebufferResized; };
		void resetWindowResizedFlag() { m_FramebufferResized = false; };

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
		void initWindow();

		int m_Width;
		int m_Height;
		bool m_FramebufferResized;

		std::string m_WindowName;
		GLFWwindow* m_Window;
	};
}