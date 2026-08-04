#pragma once

#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../utils/colors_constants.hpp"
#include "../utils/math_utils.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	glm::vec3 m_localMin, m_localMax;
	glm::vec3 m_worldMin, m_worldMax;
	glm::vec3 m_color;

	// world position, rotation and scale
	void updateWorld(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
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

		//setColor(Constants::Colors::BLACK);
	}
	glm::vec3 getSize() const { return m_worldMax - m_worldMin; };
	glm::vec3 getCenter() const { return (m_worldMin + m_worldMax) * 0.5f; };
	glm::vec3 getColor() const { return m_color; };
	void setColor(const glm::vec3& color) { m_color = color; };

	bool isInCollisionWithOther(const AABB& other) const {
		//LOG_D(Utils::getVec3Values(m_worldMin));
		//LOG_D(Utils::getVec3Values(m_worldMax));
		bool collX = (m_worldMin.x <= other.m_worldMax.x) && (m_worldMax.x >= other.m_worldMin.x);
		bool collY = (m_worldMin.y <= other.m_worldMax.y) && (m_worldMax.y >= other.m_worldMin.y);
		bool collZ = (m_worldMin.z <= other.m_worldMax.z) && (m_worldMax.z >= other.m_worldMin.z);
		return collX && collY && collZ;
	};
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent(const glm::vec3& AABBmin, const glm::vec3& AABBmax);

	void onInit() override;
	void onFixedUpdate(float fixedt) override;

	AABB getAABB() const { return m_AABB; }

private:
	TransformComponent* m_transform = nullptr;

	AABB m_AABB{};
};