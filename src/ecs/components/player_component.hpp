#pragma once

#include "../../configs/math_config.hpp"
#include "../../utils/stats_constants.hpp"
#include "i_component.hpp"

struct PlayerComponent : public IComponent {
	glm::vec3 moveDir		= glm::vec3(0.0f);
	bool isCrouching		= false;
	bool isSprinting		= false;
	bool isGodMode			= false;
	float speedWalking		= Constants::Stats::Player::SPEED_WALKING;
	float speedSprinting	= Constants::Stats::Player::SPEED_SPRINTING;
	bool isPrimary{};

	PlayerComponent() = default;
	PlayerComponent(bool isPrimary_)
		: isPrimary(isPrimary_) {}
};