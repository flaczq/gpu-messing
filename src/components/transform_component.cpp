#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "component.h"
#include "transform_component.h"

TransformComponent::TransformComponent(glm::vec3 position, glm::quat rotation, glm::vec3 scale)
	: m_position(position),
	  m_prevPosition(position),
	  m_rotation(rotation),
	  m_prevRotation(rotation),
	  m_scale(scale),
	  m_prevScale(scale)
{
}

void TransformComponent::saveState() {
	m_prevPosition = m_position;
	m_prevRotation = m_rotation;
	m_prevScale = m_scale;
}

glm::mat4 TransformComponent::getInterpolatedMatrix(float alpha) const {
	glm::vec3 interPosition = glm::mix(m_prevPosition, m_position, alpha);
	glm::quat interRotation = glm::slerp(m_prevRotation, m_rotation, alpha);
	glm::vec3 interScale = glm::mix(m_prevScale, m_scale, alpha);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), interPosition);
	model *= glm::mat4_cast(interRotation);
	model = glm::scale(model, interScale);
	return model;
}