#pragma once

#include "../../configs/math_config.hpp"
#include "../../graphics/renderer.h"
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
	void flush();
	void end();
	std::vector<RendererImmediateCommand> getAABBCommand();

private:
	std::vector<PhysicsCommand> m_physicsQueue;
	unsigned int m_VAOAABB{};
	unsigned int m_VBOAABB{};

	void updateAABB(AABB aabb, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
	bool isCollidingByAABB(AABB origin, AABB target);
	bool detectCollision(PhysicsComponent* origin, PhysicsComponent* target);
	void resolveCollisionByMTV(PhysicsCommand origin, PhysicsCommand target);
	void registerInQueue(const PhysicsCommand& command);
	glm::vec3 getAABBSize(AABB aabb) const;
	glm::vec3 getAABBCenter(AABB aabb) const;
};