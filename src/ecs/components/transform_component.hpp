#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct TransformComponent : public IComponent {
	glm::vec3 position{}, prevPosition{};
	glm::quat rotation{}, prevRotation{};
	glm::vec3 scale{}, prevScale{};
	glm::mat4 model{};
	glm::mat3 normal{};
	float yaw{};
	float pitch{};

	TransformComponent(const glm::vec3& position, const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f))
		: position(position),
		  prevPosition(position),
		  rotation(rotation),
		  prevRotation(rotation),
		  scale(scale),
		  prevScale(scale),
		  model(glm::mat4(1.0f)),
		  normal(glm::mat3(1.0f)),
		  yaw(-135.0f),
		  pitch(-11.5f) {}
};