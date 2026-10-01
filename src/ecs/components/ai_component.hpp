#pragma once

#include "i_component.hpp"

struct AIComponent : public IComponent {
	float speedMultiplier = 2.0f;

	//AIComponent() = default;
	AIComponent() {}
};