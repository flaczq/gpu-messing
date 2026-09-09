#pragma once

#include "i_component.hpp"

struct AIComponent : public IComponent {
	float speedMultiplier = 1.0f;

	AIComponent() = default;
	AIComponent(float speedMultiplier_) : speedMultiplier(speedMultiplier_) {}
};