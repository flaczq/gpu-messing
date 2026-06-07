#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class TransformComponent;
class RenderComponent;

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

	AABB getAABB() const { return m_AABB; }

private:
	TransformComponent* m_transform = nullptr;
	RenderComponent* m_render = nullptr;
	AABB m_AABB{};
};