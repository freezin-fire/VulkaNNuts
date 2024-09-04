#pragma once

#include "Window.h"
#include "SwapChain.h"
#include "Device.h"

#include <memory>
#include <vector>
#include <cassert>

namespace NNuts {
	class NNRenderer {
	public:
		NNRenderer(NNWindow &window, NNDevice &device);
		~NNRenderer();

		NNRenderer(const NNRenderer&) = delete;
		NNRenderer& operator=(const NNRenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return m_SwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress!");
			return m_CommandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		NNWindow& m_Window;
		NNDevice& m_Device;
		std::unique_ptr<NNSwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		
		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };
	};
}