#pragma once

#include "i_component.hpp"

enum class VerticalMode {
	STANDING,
	CROUCHING
};

struct StatsComponent : public IComponent {
	int health{};
	int maxHealth{};
	VerticalMode verticalMode{};

	StatsComponent() = default;
	StatsComponent(int health_,
				   int maxHealth_,
				   VerticalMode verticalMode_ = VerticalMode::STANDING)
		: health(health_),
		  maxHealth(maxHealth_),
		  verticalMode(verticalMode_) {}
};