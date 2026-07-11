#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	void update(glm::vec3 scale, glm::vec3 position) {
		min *= scale;
		min += position;
		max *= scale;
		max += position;
	}
	glm::vec3 getSize() const { return max - min; };
	glm::vec3 getCenter() const { return (min + max) * 0.5f; };
	bool isInCollisionWithOther(AABB other) const {
		bool collX = (max.x >= other.min.x) && (min.x <= other.max.x);
		bool collY = (max.y >= other.min.y) && (min.y <= other.max.y);
		bool collZ = (max.z >= other.min.z) && (min.z <= other.max.z);
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