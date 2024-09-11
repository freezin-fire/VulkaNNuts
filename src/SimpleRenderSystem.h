#pragma once

#include "Camera.h"
#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"
#include "FrameInfo.h"

#include <memory>
#include <vector>

namespace NNuts {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(NNDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(
			FrameInfo &frameInfo, 
			std::vector<NNGameObject> &gameObjects);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		NNDevice &m_Device;

		std::unique_ptr<NNPipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}