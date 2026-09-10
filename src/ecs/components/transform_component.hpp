#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct TransformComponent : public IComponent {
	glm::vec3 position		{ 0.0f };
	glm::vec3 prevPosition	{ 0.0f };
	glm::quat rotation		{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::quat prevRotation	{ 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale			{ 1.0f };
	glm::vec3 prevScale		{ 1.0f };
	//glm::mat4 model		{ 1.0f };
	//glm::mat3 normal		{ 1.0f };
	// looking at (0,0,0)
	float yaw	= -135.0f;
	float pitch	= -11.5f;

	TransformComponent() = default;
	TransformComponent(const glm::vec3& position_,
					   const glm::quat& rotation_ = glm::quat(),
					   const glm::vec3& scale_ = glm::vec3(1.0f))
		: position(position_),
		  prevPosition(position_),
		  rotation(rotation_),
		  prevRotation(rotation_),
		  scale(scale_),
		  prevScale(scale_) {}
};