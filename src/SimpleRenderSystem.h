#pragma once

#include "Camera.h"
#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"

#include <memory>
#include <vector>

namespace NNuts {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(NNDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(
			VkCommandBuffer commandBuffer, 
			std::vector<NNGameObject> &gameObjects, 
			const NNCamera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		NNDevice &m_Device;

		std::unique_ptr<NNPipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}