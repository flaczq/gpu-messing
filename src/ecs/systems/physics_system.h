#pragma once

#include "../../configs/math_config.hpp"
#include "../components/physics_component.hpp"
#include <vector>

struct PhysicsCommand {
	TransformComponent* transform;
	PhysicsComponent* physics;
};

class Registry;
class TransformComponent;

class PhysicsSystem {
public:
	PhysicsSystem();
	~PhysicsSystem();

	bool init();
	void fixedUpdate(Registry& registry, float fixedt);
	void execute();

private:
	std::vector<PhysicsCommand> m_physicsQueue{};

	void _registerInQueue(const PhysicsCommand& command);
	void _updateAABB(AABB aabb, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
	bool _detectCollision(PhysicsComponent* origin, PhysicsComponent* target);
	bool _isCollidingByAABB(AABB origin, AABB target);
	void _resolveCollisionByMTV(PhysicsCommand origin, PhysicsCommand target);
};