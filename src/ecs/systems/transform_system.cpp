#include "../../configs/math_config.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "transform_system.h"

TransformSystem::TransformSystem() = default;

void TransformSystem::saveState(Registry& registry) {
	m_prevPosition = m_position;
	m_prevRotation = m_rotation;
	m_prevScale = m_scale;
}

void TransformSystem::updateRotation(Registry& registry) {
	glm::quat qYaw = glm::angleAxis(glm::radians(-m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat qPitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	m_rotation = qYaw * qPitch;
	//m_prevRotation = m_rotation;
}

glm::mat4 TransformSystem::getInterpolatedModelMatrix(Registry& registry, float alpha) {
	glm::vec3 interPosition = glm::mix(m_prevPosition, m_position, alpha);
	glm::quat interRotation = glm::slerp(m_prevRotation, m_rotation, alpha);
	glm::vec3 interScale = glm::mix(m_prevScale, m_scale, alpha);
	m_model = glm::translate(glm::mat4(1.0f), interPosition);
	m_model *= glm::mat4_cast(interRotation);
	m_model = glm::scale(m_model, interScale);
	return m_model;
}

glm::mat4 TransformSystem::getNormalMatrix(Registry& registry) {
	m_normal = glm::transpose(glm::inverse(glm::mat3(m_model)));
	return m_normal;
}

glm::vec3 TransformSystem::getFront(Registry& registry) {
	glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
	front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	front.y = sin(glm::radians(m_pitch));
	front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	return glm::normalize(front);
}

glm::vec3 TransformSystem::getFlatFront() const {
	glm::vec3 flatFront = glm::vec3(0.0f, 0.0f, 0.0f);
	flatFront.x = cos(glm::radians(m_yaw));
	flatFront.y = 0.0f;
	flatFront.z = sin(glm::radians(m_yaw));
	return glm::normalize(flatFront);
}

glm::vec3 TransformSystem::getRight() const {
	return glm::normalize(glm::cross(getFront(), Constants::Math::WORLD_UP));
}

glm::vec3 TransformSystem::getUp() const {
	return glm::normalize(glm::cross(getRight(), getFront()));
}