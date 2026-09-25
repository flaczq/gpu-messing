#pragma once

#include "../../configs/math_config.hpp"
#include "../components/identity_component.hpp"
#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include <vector>

struct PhysicsCommand {
	IdentityComponent& identity;
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
	glm::vec3 _findMinimumTranslationVector(const AABB& aabbA, const AABB& aabbB);
	void _resolveCollisionWithMTV(PhysicsCommand& commandX, PhysicsCommand& commandY, const glm::vec3& mtv);
};