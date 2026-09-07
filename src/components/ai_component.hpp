#pragma once

#include "i_component.hpp"

struct AIComponent : public IComponent {
	bool enabled = false;

	AIComponent(bool enabled) : enabled(enabled) {}
};