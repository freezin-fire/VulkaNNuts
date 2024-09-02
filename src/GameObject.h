#pragma once

#include "Model.h"

#include <memory>

namespace NNuts {
	struct Trasform2dComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMat{ {c, s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} }; //takes in columns and not rows
			return rotMat * scaleMat;
		}
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
		Trasform2dComponent transform2d;

	private:
		NNGameObject(id_t objId) : id{ objId } {}
	};
}