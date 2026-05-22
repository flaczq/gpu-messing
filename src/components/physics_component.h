#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent;

struct AABB {
	void update(const glm::vec3& position, float width, float height, float depth) {
		min = { position.x - width / 2, position.y - height / 2, position.z - depth / 2 };
		max = { position.x + width / 2, position.y + height / 2, position.z + depth / 2 };
	};

	glm::vec3 min;
	glm::vec3 max;
};

class PhysicsComponent : public Component {
public:
	PhysicsComponent();

	void onInit() override;
	void onFixedUpdate(float fixedt) override;

private:
	TransformComponent* m_transform = nullptr;
	AABB aabb{};
};