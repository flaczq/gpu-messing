#pragma once

#include "i_component.hpp"

struct PlayerComponent : public IComponent {
	bool isPrimary{};

	PlayerComponent(bool isPrimary_ = true)
		: isPrimary(isPrimary_) {}
};