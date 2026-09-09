#pragma once

#include "../../configs/math_config.hpp"
#include <vector>

struct PhysicsCommand {
	TransformComponent* transform;
	PhysicsComponent* physics;
};

class Registry;
class TransformComponent;
class PhysicsComponent;

class PhysicsSystem {
public:
	PhysicsSystem();

	void init();
	void fixedUpdate(Registry& registry, float fixedt);
	//glm::vec3 getSize() const { return worldMax - worldMin; };
	//glm::vec3 getCenter() const { return (worldMin + worldMax) * 0.5f; };

private:
	std::vector<PhysicsCommand> m_physicsQueue;

	void updateAABB(AABB aabb, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
	void registerInQueue(const PhysicsCommand& command);
};