#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "component.h"
#include "transform_component.h"

TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
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
	glm::mat4 model = glm::translate(glm::mat4(1.0f), interPosition);
	model = glm::rotate(model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, m_scale);
	return model;
}