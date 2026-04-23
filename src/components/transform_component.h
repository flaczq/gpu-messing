#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent : public Component {
public:
	TransformComponent(glm::vec3 position, glm::quat rotation = glm::quat(), glm::vec3 scale = glm::vec3(1.0f));

	void saveState();
	glm::mat4 getInterpolatedModelMatrix(float alpha);
	glm::mat4 getNormalMatrix(glm::mat4 modelMatrix);

	void setDirty(bool dirty) { m_dirty = dirty; }
	void setRotation(glm::quat rotation) { m_rotation = rotation; m_dirty = true; }
	void setScale(glm::vec3 scale) { m_scale = scale; m_dirty = true; }
	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 position) { m_position = position; m_dirty = true; }

private:
	glm::mat4 m_model{};
	glm::mat3 m_normal{};
	glm::vec3 m_position{}, m_prevPosition{};
	glm::vec3 m_scale{}, m_prevScale{};
	glm::quat m_rotation{}, m_prevRotation{};
	
	bool m_dirty = true;
};