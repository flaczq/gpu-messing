#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "component.h"
#include "transform_component.h"

TransformComponent::TransformComponent(glm::vec3 startPosition)
	: m_position(startPosition),
	  m_previousPosition(startPosition)
{
}

void TransformComponent::saveState() {
	m_previousPosition = m_position;
}

glm::mat4 TransformComponent::getInterpolatedMatrix(float alpha) const {
	glm::vec3 interPos = glm::mix(m_previousPosition, m_position, alpha);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), interPos);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(100.0f));
	return model;
}