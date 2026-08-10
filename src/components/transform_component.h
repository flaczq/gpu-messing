#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent : public Component {
public:
	TransformComponent(const glm::vec3& position, const glm::quat& rotation = glm::quat(), const glm::vec3& scale = glm::vec3(1.0f));

	virtual void saveState();
	glm::vec3 getInterpolatedPosition(float alpha) const;
	virtual glm::mat4 getInterpolatedModelMatrix(float alpha);
	virtual glm::mat4 getNormalMatrix();
	glm::vec3 getFront() const;
	glm::vec3 getFlatFront() const;
	glm::vec3 getUp() const;
	glm::vec3 getRight() const;

	virtual glm::mat4 getModel() const { return m_model; }
	virtual glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 position) { m_position = position; m_dirty = true; }
	void addPosition(glm::vec3 position) { m_position += position; m_dirty = true; }
	glm::quat getRotation() const { return m_rotation; }
	void setRotation(glm::quat rotation) { m_rotation = rotation; m_dirty = true; }
	glm::vec3 getScale() const { return m_scale; }
	void setScale(glm::vec3 scale) { m_scale = scale; m_dirty = true; }
	float getYaw() const { return m_yaw; }
	void setYaw(float yaw) { m_yaw = yaw; }
	void addYaw(float yaw) { m_yaw += yaw; }
	float getPitch() const { return m_pitch; }
	void setPitch(float pitch) { m_pitch = pitch; }
	void addPitch(float pitch) { m_pitch += pitch; }
	void setDirty(bool dirty) { m_dirty = dirty; }
	glm::vec3 getWorldUp() const { return WORLD_UP; }

protected:
	static constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::mat4 m_model{};
	glm::mat3 m_normal{};
	glm::vec3 m_position{}, m_prevPosition{};
	glm::quat m_rotation{}, m_prevRotation{};
	glm::vec3 m_scale{}, m_prevScale{};
	float m_yaw{};
	float m_pitch{};
	
	bool m_dirty = true;
};