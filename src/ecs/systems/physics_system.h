#pragma once

#include "../../configs/math_config.hpp"
#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include <vector>

struct PhysicsCommand {
	TransformComponent& transform;
	PhysicsComponent& physics;
};

class Registry;

class PhysicsSystem {
public:
	PhysicsSystem();

	bool init();
	void fixedUpdate(Registry& registry, float fixedt);
	void execute();

private:
	std::vector<PhysicsCommand> m_physicsQueue{};

	void _registerInQueue(const PhysicsCommand& command);
	void _updateAABB(AABB& aabb, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
	bool _detectCollision(const PhysicsComponent& physicsX, const PhysicsComponent& physicsY);
	bool _isCollidingByAABB(const AABB& aabbX, const AABB& aabbY);
	void _resolveCollisionByMTV(PhysicsCommand& commandX, PhysicsCommand& commandY);
};