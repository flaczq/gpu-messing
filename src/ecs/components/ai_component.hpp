#pragma once

#include "i_component.hpp"

struct AIComponent : public IComponent {
	float speedMultiplier = 1.0f;

	//AIComponent(float speedMultiplier) : speedMultiplier(speedMultiplier) {}
};