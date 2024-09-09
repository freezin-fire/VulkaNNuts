#include "Application.h"

#include "Camera.h"
#include "SimpleRenderSystem.h"
#include "KeyboardMovementController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

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
		NNCamera camera{};
		camera.setViewDirection(glm::vec3{ 0.0f }, glm::vec3{ 0.0f, 0.0f, 1.0f });

		auto viewerObject = NNGameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!m_Window.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(m_Window.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = m_Renderer.getAspectRatio();
			//camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
			camera.setPerespectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.f);

			if (auto commandBuffer = m_Renderer.beginFrame())
			{
				m_Renderer.beginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, m_GameObjects, camera);
				m_Renderer.endSwapChainRenderPass(commandBuffer);
				m_Renderer.endFrame();
			}

		}

		vkDeviceWaitIdle(m_Device.device());
	}
	
	std::unique_ptr<NNModel> createCubeModel(NNDevice& device, glm::vec3 offset) {
		NNModel::Builder modelBuilder{};

		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

		modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
						  12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<NNModel>(device, modelBuilder);
	}

	void NNApplication::loadGameObjects()
	{
		std::shared_ptr<NNModel> model = NNModel::createModelFromFile(m_Device, "res/Models/flat_vase.obj");

		auto gameObj = NNGameObject::createGameObject();
		gameObj.model = model;
		gameObj.transform.translation = { 0.0f, 0.5f, 2.5f };
		gameObj.transform.scale = { 3.0f, 1.5f, 2.0f };
		//gameObj.transform.scale = glm::vec3{3.0f};
		m_GameObjects.push_back(std::move(gameObj));
	}
}