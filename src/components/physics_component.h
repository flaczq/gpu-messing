#pragma once

#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../utils/math_utils.hpp"
#include "component.h"

class TransformComponent;

// <=
// BOT collides w/ only itself
// MID collides w/ MID and BOT
// TOP collides w/ every other
enum class PhysicsLayer {
	BOT = 1,
	MID = 2,
	TOP = 3
};

struct AABB {
	glm::vec3 m_localMin, m_localMax;
	glm::vec3 m_worldMin, m_worldMax;

	// world position, rotation and scale
	void updateToWorld(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);

		// AABB corners
		glm::vec3 corners[8] = {
			{ m_localMin.x, m_localMin.y, m_localMin.z }, { m_localMax.x, m_localMin.y, m_localMin.z },
			{ m_localMin.x, m_localMax.y, m_localMin.z }, { m_localMax.x, m_localMax.y, m_localMin.z },
			{ m_localMin.x, m_localMin.y, m_localMax.z }, { m_localMax.x, m_localMin.y, m_localMax.z },
			{ m_localMin.x, m_localMax.y, m_localMax.z }, { m_localMax.x, m_localMax.y, m_localMax.z }
		};

		m_worldMin = glm::vec3(std::numeric_limits<float>::max());
		m_worldMax = glm::vec3(std::numeric_limits<float>::lowest());
		for (size_t i{}; i < 8; i++) {
			glm::vec3 corner = glm::vec3(model * glm::vec4(corners[i], 1.0f));
			m_worldMin = glm::min(m_worldMin, corner);
			m_worldMax = glm::max(m_worldMax, corner);
		}
	}
	glm::vec3 getWorldMin() const { return m_worldMin; };
	glm::vec3 getWorldMax() const { return m_worldMax; };
	glm::vec3 getSize() const { return m_worldMax - m_worldMin; };
	glm::vec3 getCenter() const { return (m_worldMin + m_worldMax) * 0.5f; };
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent(const glm::vec3& AABBmin, const glm::vec3& AABBmax, PhysicsLayer layer = PhysicsLayer::BOT);

	void onInit() override;
	void onFixedUpdate(float fixedt) override;

	AABB getAABB() const { return m_AABB; }
	PhysicsLayer getLayer() const { return m_layer; }
	bool isColliding() const { return m_colliding; }
	void setColliding(bool colliding) { m_colliding = colliding; }

private:
	TransformComponent* m_transform = nullptr;

	AABB m_AABB{};
	PhysicsLayer m_layer{};
	bool m_colliding{};
};