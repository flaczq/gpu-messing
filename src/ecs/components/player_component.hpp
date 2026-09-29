#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct PlayerComponent : public IComponent {
	glm::vec3 moveDir	= glm::vec3(0.0f);
	bool isCrouching	= false;
	bool isGodMode		= false;
	float speed{};
	bool isPrimary{};

	//PlayerComponent() = default;
	PlayerComponent(float speed_ = 0.0f,
					bool isPrimary_ = true)
		: speed(speed_),
		  isPrimary(isPrimary_) {}
};