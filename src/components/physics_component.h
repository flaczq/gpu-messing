#pragma once

#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../utils/math_utils.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	glm::vec3 localMin, localMax;
	glm::vec3 worldMin, worldMax;

	// world model
	void updateWorld(const glm::mat4& model) {
		glm::vec3 localCenter = (localMin + localMax) * 0.5f;
		glm::vec3 localSize = (worldMax - worldMin) * 0.5f;
		glm::vec3 worldCenter = glm::vec3(model * glm::vec4(localCenter, 1.0f));

		glm::vec3 worldSize(0.0f);
		// 1st column: X-axis * X-scale
		// 2nd column: Y-axis * Y-scale
		// 3rd column: Z-axis * Z-scale
		for (size_t i{}; i < 3; i++) {
			glm::vec3 column = glm::vec3(model[i]);
			// abs() to convert negative values caused by rotation
			worldSize += glm::abs(column) * localSize[i];
		}

		worldMin = worldCenter - worldSize;
		worldMax = worldCenter + worldSize;
	}
	// world position, rotation and scale
	void updateWorld(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);

		updateWorld(model);
	}
	glm::vec3 getSize() const { return worldMax - worldMin; };
	glm::vec3 getCenter() const { return (worldMin + worldMax) * 0.5f; };
	bool isInCollisionWithOther(const AABB& other) const {
		//LOG_D(Utils::getVec3Values(worldMin));
		//LOG_D(Utils::getVec3Values(worldMax));
		bool collX = (worldMin.x <= other.worldMax.x) && (worldMax.x >= other.worldMin.x);
		bool collY = (worldMin.y <= other.worldMax.y) && (worldMax.y >= other.worldMin.y);
		bool collZ = (worldMin.z <= other.worldMax.z) && (worldMax.z >= other.worldMin.z);
		return collX && collY && collZ;
	};
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent(glm::vec3 AABBmin, glm::vec3 AABBmax);

	void onInit() override;
	void onFixedUpdate(float fixedt) override;

	AABB getAABB() const { return m_AABB; }

private:
	TransformComponent* m_transform = nullptr;
	AABB m_AABB{};
};