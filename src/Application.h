#pragma once

#include "Descriptor.h"
#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Window.h"

#include <memory>
#include <vector>

namespace NNuts {
	class NNApplication {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		NNApplication();
		~NNApplication();

		NNApplication(const NNApplication&) = delete;
		NNApplication& operator=(const NNApplication&) = delete;

		void run();

	private:
		void loadGameObjects();

		NNWindow m_Window{ WIDTH, HEIGHT, "TESTING THESE NNUTS!" };
		NNDevice m_Device{ m_Window };
		NNRenderer	m_Renderer{ m_Window, m_Device };

		std::unique_ptr<NNDescriptorPool> m_GlobalPool;
		std::vector<NNGameObject> m_GameObjects;
	};
}