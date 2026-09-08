#pragma once

#include "../../configs/math_config.hpp"

class Registry;

class PlayerSystem {
public:
	PlayerSystem();

	void processInput(Registry& registry);
	void fixedUpdate(Registry& registry, float fixedt);
	void toggleVerticalMode(Registry& registry);
	void toggleGodMode(Registry& registry);
	bool isCrouching(Registry& registry) const;

private:
	static constexpr float MOVEMENT_SPEED = 5.0f;

	glm::vec3 m_moveDir = glm::vec3(0.0f);
	bool m_verticalModeChanged = false;
	bool m_godMode = false;
	bool m_godModeChanged = false;
};