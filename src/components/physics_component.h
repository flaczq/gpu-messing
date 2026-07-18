#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	// local
	glm::vec3 min, max;
	glm::vec3 worldMin, worldMax;

	// global position with rotation and scale
	void updateGlobal(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
		//worldMin = min * scale;
		//worldMin += position;
		//worldMax = max * scale;
		//worldMax += position;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model *= glm::mat4_cast(rotation);
		model = glm::scale(model, scale);

		//AABB box
		glm::vec3 corners[8] = {
			{min.x, min.y, min.z}, {max.x, min.y, min.z},
			{min.x, max.y, min.z}, {max.x, max.y, min.z},
			{min.x, min.y, max.z}, {max.x, min.y, max.z},
			{min.x, max.y, max.z}, {max.x, max.y, max.z}
		};

		worldMin = glm::vec3(std::numeric_limits<float>::max());
		worldMax = glm::vec3(-std::numeric_limits<float>::min());

		for (int i = 0; i < 8; ++i) {
			glm::vec3 transformed = glm::vec3(model * glm::vec4(corners[i], 1.0f));
			worldMin = glm::min(worldMin, transformed);
			worldMax = glm::max(worldMax, transformed);
		}
	}
	glm::vec3 getSize() const { return worldMax - worldMin; };
	glm::vec3 getCenter() const { return (worldMin + worldMax) * 0.5f; };
	bool isInCollisionWithOther(AABB other) const {
		bool collX = (worldMax.x >= other.worldMin.x) && (worldMin.x <= other.worldMax.x);
		bool collY = (worldMax.y >= other.worldMin.y) && (worldMin.y <= other.worldMax.y);
		bool collZ = (worldMax.z >= other.worldMin.z) && (worldMin.z <= other.worldMax.z);
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