#pragma once

#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace NNuts {
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation;

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	class NNGameObject {
	public:
		using id_t = unsigned int;

		static NNGameObject createGameObject() {
			static id_t currentId = 0;
			return NNGameObject{ currentId++ };
		}

		NNGameObject(const NNGameObject&) = delete;
		NNGameObject &operator=(const NNGameObject&) = delete;
		NNGameObject(NNGameObject&&) = default;
		NNGameObject &operator=(NNGameObject&&) = default;

		const id_t id;
		std::shared_ptr<NNModel> model;
		glm::vec3 color;
		TransformComponent transform{};

	private:
		NNGameObject(id_t objId) : id{ objId } {}
	};
}