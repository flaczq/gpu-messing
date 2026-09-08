#pragma once

#include "i_component.hpp"

enum class VerticalMode {
	STANDING,
	CROUCHING
};

struct PlayerComponent : public IComponent {
	int health = 50;
	VerticalMode verticalMode = VerticalMode::STANDING;
};