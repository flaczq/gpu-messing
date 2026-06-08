#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	glm::vec3 getSize() const { return max - min; };
	glm::vec3 getCenter() const { return (min + max) * 0.5f; };
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