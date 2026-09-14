#pragma once

#include "../../configs/math_config.hpp"
#include "../components/player_component.hpp"

class Registry;

class PlayerSystem {
public:
	PlayerSystem();

	void processInput(Registry& registry);
	void fixedUpdate(Registry& registry, float fixedt);

private:
	void _toggleCrouching(PlayerComponent* player);
	void _toggleGodMode(PlayerComponent* player);
};