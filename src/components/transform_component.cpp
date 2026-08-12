#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../utils/math_constants.hpp"
#include "component.h"
#include "transform_component.h"

TransformComponent::TransformComponent(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
	: m_position(position),
	  m_prevPosition(position),
	  m_rotation(rotation),
	  m_prevRotation(rotation),
	  m_scale(scale),
	  m_prevScale(scale),
	  m_model(glm::mat4(1.0f)),
	  m_normal(glm::mat3(1.0f)),
	  // looking at (0,0,0)
	  m_yaw(-135.0f),
	  m_pitch(-11.5f),
	  m_dirty(true)
{
}

void TransformComponent::saveState() {
	m_prevPosition = m_position;
	m_prevRotation = m_rotation;
	m_prevScale = m_scale;
}

void TransformComponent::updateRotation() {
	glm::quat qYaw = glm::angleAxis(glm::radians(-m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	m_rotation = qYaw * qPitch;
	//m_prevRotation = m_rotation;
}

glm::mat4 TransformComponent::getInterpolatedModelMatrix(float alpha) {
	if (m_dirty) {
		glm::vec3 interPosition = glm::mix(m_prevPosition, m_position, alpha);
		glm::quat interRotation = glm::slerp(m_prevRotation, m_rotation, alpha);
		glm::vec3 interScale = glm::mix(m_prevScale, m_scale, alpha);
		m_model = glm::translate(glm::mat4(1.0f), interPosition);
		m_model *= glm::mat4_cast(interRotation);
		m_model = glm::scale(m_model, interScale);
		//m_dirty = false;
	}
	return m_model;
}

glm::mat4 TransformComponent::getNormalMatrix() {
	if (m_dirty) {
		m_normal = glm::transpose(glm::inverse(glm::mat3(m_model)));
		//m_dirty = false;
	}
	return m_normal;
}

glm::vec3 TransformComponent::getInterpolatedPosition(float alpha) const {
	return glm::mix(m_prevPosition, m_position, alpha);
}

glm::vec3 TransformComponent::getFront() const {
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	return glm::normalize(front);
}

glm::vec3 TransformComponent::getFlatFront() const {
	glm::vec3 flatFront = glm::vec3(0.0f, 0.0f, 0.0f);
	flatFront.x = cos(glm::radians(m_yaw));
	flatFront.y = 0.0f;
	flatFront.z = sin(glm::radians(m_yaw));
	return glm::normalize(flatFront);
}


glm::vec3 TransformComponent::getRight() const {
	return glm::normalize(glm::cross(getFront(), Constants::Math::WORLD_UP));
}

glm::vec3 TransformComponent::getUp() const {
	return glm::normalize(glm::cross(getRight(), getFront()));
}