#pragma once

#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../utils/math_utils.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	glm::vec3 localMin, localMax;
	glm::vec3 worldMin, worldMax;

	// world position, rotation and scale
	void updateWorld(glm::vec3 position, glm::quat rotation, glm::vec3 scale) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);

		// AABB corners
		glm::vec3 corners[8] = {
			{ localMin.x, localMin.y, localMin.z }, { localMax.x, localMin.y, localMin.z },
			{ localMin.x, localMax.y, localMin.z }, { localMax.x, localMax.y, localMin.z },
			{ localMin.x, localMin.y, localMax.z }, { localMax.x, localMin.y, localMax.z },
			{ localMin.x, localMax.y, localMax.z }, { localMax.x, localMax.y, localMax.z }
		};

		worldMin = glm::vec3(std::numeric_limits<float>::max());
		worldMax = glm::vec3(std::numeric_limits<float>::lowest());
		for (size_t i{}; i < 8; i++) {
			glm::vec3 corner = glm::vec3(model * glm::vec4(corners[i], 1.0f));
			worldMin = glm::min(worldMin, corner);
			worldMax = glm::max(worldMax, corner);
		}
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