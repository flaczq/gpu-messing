#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent : public Component {
public:
	TransformComponent(glm::vec3 position, glm::quat rotation, glm::vec3 scale);

	void saveState();
	glm::mat4 getInterpolatedMatrix(float alpha) const;

	void setRotation(glm::quat rotation) { m_rotation = rotation; m_prevRotation = rotation; }

private:
	glm::vec3 m_position{}, m_prevPosition{};
	glm::vec3 m_scale{}, m_prevScale{};
	glm::quat m_rotation{}, m_prevRotation{};
};