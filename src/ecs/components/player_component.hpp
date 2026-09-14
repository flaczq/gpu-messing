#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

struct PlayerComponent : public IComponent {
	glm::vec3 moveDir	= glm::vec3(0.0f);
	bool isCrouching	= false;
	bool isGodMode		= false;
	bool isPrimary{};

	//PlayerComponent() = default;
	PlayerComponent(bool isPrimary_ = true)
		: isPrimary(isPrimary_) {}
};