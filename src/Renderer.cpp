#include "Renderer.h"

#include <stdexcept>
#include <array>

namespace NNuts {
	NNRenderer::NNRenderer(NNWindow& window, NNDevice& device)
		:m_Window{window}, m_Device{device}
	{
		recreateSwapChain();
		createCommandBuffers();
	}

	NNRenderer::~NNRenderer()
	{
		freeCommandBuffers();
	}

	void NNRenderer::recreateSwapChain()
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
			std::shared_ptr<NNSwapChain> oldSwapChain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<NNSwapChain>(m_Device, extent, std::move(m_SwapChain));

			if (!oldSwapChain->compareSwapFormats(*m_SwapChain.get())) {
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}

	void NNRenderer::createCommandBuffers()
	{
		m_CommandBuffers.resize(NNSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.device(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allcoate Command Buffers!");
		}

	}

	void NNRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_Device.device(), 
			m_Device.getCommandPool(), 
			static_cast<uint32_t>(m_CommandBuffers.size()), 
			m_CommandBuffers.data()
		);
		m_CommandBuffers.clear();
	}

	VkCommandBuffer NNRenderer::beginFrame()
	{
		assert(!isFrameStarted && "Can't call beginFrame while already in progress!");
		
		auto result = m_SwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire Swap Chain Image!");
		}

		isFrameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void NNRenderer::endFrame()
	{
		assert(isFrameStarted && "Can't call endFrame while already is not in progress!");
		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = m_SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.wasWindowResized()) {
			m_Window.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to acquire Swap Chain Image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % NNSwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void NNRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass while frame is not in progress!");
		assert(
			commandBuffer == getCurrentCommandBuffer() && 
			"Can't begin renderPass on command buffer from different frame!");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->getRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, m_SwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	}

	void NNRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass while frame already in progress!");
		assert(
			commandBuffer == getCurrentCommandBuffer() && 
			"Can't end renderPass on command buffer from different frame!");
		vkCmdEndRenderPass(commandBuffer);
	}

}