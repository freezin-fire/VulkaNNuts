#include "Application.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <cassert>
#include <array>

namespace NNuts {
	struct SimplePushConstantData {
		glm::mat2 transform{ 1.0f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	Application::Application()
	{
		loadGameObjects();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(m_Device.device(), m_PipelineLayout, nullptr);
	}

	//void Application::sierpinski(std::vector<NNModel::Vertex>& vertices, int depth, glm::vec2 left, glm::vec2 right, glm::vec2 top)
	//{
	//	if (depth <= 0) {
	//		vertices.push_back({ top });
	//		vertices.push_back({ right });
	//		vertices.push_back({ left });
	//	}
	//	else {
	//		auto leftTop = 0.5f * (left + top);
	//		auto rightTop = 0.5f * (right + top);
	//		auto leftRight = 0.5f * (left + right);

	//		sierpinski(vertices, depth - 1, left, leftRight, leftTop);
	//		sierpinski(vertices, depth - 1, leftRight, right, rightTop);
	//		sierpinski(vertices, depth - 1, leftTop, rightTop, top);
	//	}
	//}
	
	void Application::loadGameObjects()
	{
		// triangle test vertices
		std::vector<NNModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		// sierpinski code
		//std::vector<NNModel::Vertex> vertices{};
		//sierpinski(vertices, 6, { 0.0f, -0.9f }, { 0.9f, 0.9f }, { -0.9f, 0.9f });

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

		for (int  i = 0; i < 40; i++)
		{
			auto triangle = NNGameObject::createGameObject();
			triangle.model = model;
			triangle.transform2d.scale = glm::vec2(0.5f) + i * 0.025f;
			triangle.transform2d.rotation = i * glm::pi<float>() * 0.025f;
			triangle.color = colors[i % colors.size()];
			m_GameObjects.push_back(std::move(triangle));
		}

	}

	void Application::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(m_Device.device(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) !=
			VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout!");
		}
	}
	
	void Application::createPipeline()
	{
		assert(m_SwapChain != nullptr && "Cannot create pipeline before swap chain!");
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout!");
		PipelineConfigInfo pipelineConfig{};
		NNPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = m_SwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<NNPipeline>(
			m_Device,
			"res/Shaders/BasicShader.vert.spv",
			"res/Shaders/BasicShader.frag.spv",
			pipelineConfig
		);
	}

	void Application::recreateSwapChain()
	{
		auto extent = m_Window.getExtent();
		while (extent.width == 0 || extent.height == 0){
			extent = m_Window.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device.device());
		if (m_SwapChain == nullptr) {
			m_SwapChain = std::make_unique<NNSwapChain>(m_Device, extent);
		}
		else {
			m_SwapChain = std::make_unique<NNSwapChain>(m_Device, extent, std::move(m_SwapChain));
			if (m_SwapChain->imageCount() != m_CommandBuffers.size())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline();
	}

	void Application::createCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allcoate Command Buffers!");
		}
	}

	void Application::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_Device.device(), 
			m_Device.getCommandPool(), 
			static_cast<uint32_t>(m_CommandBuffers.size()), 
			m_CommandBuffers.data()
		);
		m_CommandBuffers.clear();
	}
	
	void Application::recordCommandBuffer(int imageIndex)
	{
		static int frame = 0;

		frame = (frame + 1) % 100;


		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
		vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

		renderGameObjects(m_CommandBuffers[imageIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void Application::renderGameObjects(VkCommandBuffer commandBuffer) {
		// update
		int i = 0;
		for (auto& obj : m_GameObjects) {
			i += 1;
			obj.transform2d.rotation = glm::mod<float>(obj.transform2d.rotation + 0.001f * i, 2.0f * glm::pi<float>());
		}

		// render
		m_Pipeline->bind(commandBuffer);
		for (auto& obj : m_GameObjects)
		{
			//obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = obj.transform2d.translation;
			push.color = obj.color;
			push.transform = obj.transform2d.mat2();

			vkCmdPushConstants(
				commandBuffer, 
				m_PipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
				0, 
				sizeof(SimplePushConstantData), 
				&push);
			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}


	}
	
	void Application::drawFrame()
	{
		uint32_t imageIndex;
		auto result = m_SwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire Swap Chain Image!");
		}

		recordCommandBuffer(imageIndex);
		result = m_SwapChain->submitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized()) {
			m_Window.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to acquire Swap Chain Image!");
		}
	}
}