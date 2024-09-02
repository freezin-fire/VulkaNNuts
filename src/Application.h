#pragma once

#include "Window.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include "Device.h"
#include "GameObject.h"

#include <memory>
#include <vector>

namespace NNuts {
	class Application {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run()
		{	
			while (!m_Window.shouldClose()) {
				glfwPollEvents();
				drawFrame();
			}

			vkDeviceWaitIdle(m_Device.device());
		}

		//sierpinski triangle generator function
		void sierpinski(std::vector<NNModel::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top);

	private:
		void loadGameObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		NNWindow m_Window{ WIDTH, HEIGHT, "TESTING THESE NNUTS!" };
		NNDevice m_Device{ m_Window };
		std::unique_ptr<NNSwapChain> m_SwapChain;
		std::unique_ptr<NNPipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<NNGameObject> m_GameObjects;
	};
}