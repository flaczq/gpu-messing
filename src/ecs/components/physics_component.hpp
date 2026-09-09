#pragma once

#include "../../configs/math_config.hpp"
#include "i_component.hpp"

// <=
enum class PhysicsLayer {
	BOT = 1, // collides only with itself
	MID = 2, // collides with MID and BOT
	TOP = 3  // collides with every other
};

struct AABB {
	glm::vec3 localMin{};
	glm::vec3 localMax{};
	glm::vec3 worldMin{};
	glm::vec3 worldMax{};
};

struct PhysicsComponent : public IComponent {
	AABB AABB{};
	PhysicsLayer layer{};
	bool colliding{};

	PhysicsComponent(const glm::vec3& AABBmin, const glm::vec3& AABBmax, PhysicsLayer layer = PhysicsLayer::BOT)
		: AABB(AABBmin, AABBmax, AABBmin, AABBmax),
		  layer(layer),
		  colliding(false) {}
};