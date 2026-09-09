#pragma once

#include "../configs/math_config.hpp"
#include "../graphics/renderer.h"
#include <unordered_map>
#include <vector>

class TransformComponent;
class PhysicsComponent;

class PhysicsWorld {
public:
	// Meyer's Singleton
	static PhysicsWorld& getInstance();
	PhysicsWorld(const PhysicsWorld&) = delete;
	void operator=(const PhysicsWorld&) = delete;
	~PhysicsWorld();

	bool init();
	void registerInQueue(const PhysicsCommand& command);
	void flush();
	void step(float fixedt);
	bool isCollidingByAABB(AABB origin, AABB target);
	bool detectCollision(PhysicsComponent* origin, PhysicsComponent* target);
	void resolveCollisionByMTV(PhysicsBody origin, PhysicsBody target);
	void end();

	std::vector<RendererImmediateCommand> getAABBCommand();

private:
	// hidden constructor
	PhysicsWorld();

	std::vector<PhysicsCommand> m_physicsQueue;
	std::unordered_map<std::string, PhysicsBody> m_physicsBodies;

	unsigned int m_VAOAABB{}, m_VBOAABB{};
};