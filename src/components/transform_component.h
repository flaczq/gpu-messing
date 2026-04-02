#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent : public Component {
public:
	TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	void saveState();
	glm::mat4 getInterpolatedMatrix(float alpha) const;

	void setRotation(glm::vec3 rotation) { m_rotation = rotation; m_prevRotation = rotation; }

private:
	glm::vec3 m_position{}, m_prevPosition{};
	glm::vec3 m_scale{}, m_prevScale{};
	glm::vec3 m_rotation{}, m_prevRotation{};
};