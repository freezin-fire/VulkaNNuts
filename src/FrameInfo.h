#pragma once

#include "Camera.h"

#include <vulkan/vulkan.h>

namespace NNuts {
	struct FrameInfo
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		NNCamera& camera;
		VkDescriptorSet globalDescriptorSet;
	};
}