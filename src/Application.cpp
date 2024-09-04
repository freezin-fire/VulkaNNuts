#include "Application.h"

#include "SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <cassert>
#include <array>

namespace NNuts {
	NNApplication::NNApplication()
	{
		loadGameObjects();
	}

	NNApplication::~NNApplication()
	{
	}

	void NNApplication::run()
	{
		SimpleRenderSystem simpleRenderSystem(m_Device, m_Renderer.getSwapChainRenderPass());
		while (!m_Window.shouldClose()) {
			glfwPollEvents();

			if (auto commandBuffer = m_Renderer.beginFrame())
			{
				m_Renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, m_GameObjects);
				m_Renderer.endSwapChainRenderPass(commandBuffer);
				m_Renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(m_Device.device());
	}

	void NNApplication::loadGameObjects()
	{
		// triangle test vertices
		std::vector<NNModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		auto model = std::make_shared<NNModel>(m_Device, vertices);

		std::vector<glm::vec3> colors{
			{1.0f, 0.7f, 0.73f},
			{1.0f, 0.87f, 0.73f},
			{1.0f, 1.0f, 0.73f},
			{0.73f, 1.0f, 0.8f},
			{0.73f, 0.88f, 1.0f}
		};

		for (auto& color : colors) {
			color = glm::pow(color, glm::vec3{ 2.2f });
		}

		for (int i = 0; i < 40; i++)
		{
			auto triangle = NNGameObject::createGameObject();
			triangle.model = model;
			triangle.transform2d.scale = glm::vec2(0.5f) + i * 0.025f;
			triangle.transform2d.rotation = i * glm::pi<float>() * 0.025f;
			triangle.color = colors[i % colors.size()];
			m_GameObjects.push_back(std::move(triangle));
		}

	}
}