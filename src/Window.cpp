#include "Window.h"

namespace NNuts {
	NNWindow::NNWindow(int w, int h, std::string name):
		m_Width(w), m_Height(h), m_WindowName(name)
	{
		initWindow();
	}

	NNWindow::~NNWindow()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}
	
	void NNWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface));
	}

	void NNWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto t_Window = reinterpret_cast<NNWindow*>(glfwGetWindowUserPointer(window));
		t_Window->m_FramebufferResized = true;
		t_Window->m_Width = width;
		t_Window->m_Height = height;
	}

	void NNWindow::initWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, framebufferResizeCallback);
	}
}