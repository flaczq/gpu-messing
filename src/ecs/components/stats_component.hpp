#pragma once

#include "i_component.hpp"

struct StatsComponent : public IComponent {
	int health{};
	int maxHealth{};

	StatsComponent() = default;
	StatsComponent(int health_,
				   int maxHealth_)
		: health(health_),
		  maxHealth(maxHealth_) {}
};