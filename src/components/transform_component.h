#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent : public Component {
public:
	TransformComponent(glm::vec3 startPosition);

	void saveState();
	glm::mat4 getInterpolatedMatrix(float alpha) const;

private:
	glm::vec3 m_position;
	glm::vec3 m_previousPosition;
	// TODO same for rotation, scale
};