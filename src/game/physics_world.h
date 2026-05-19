#pragma once

#include <vector>

class TransformComponent;

struct PhysicsCommand {
	TransformComponent* transform;
};

class PhysicsWorld {
public:
	// Meyer's Singleton
	static PhysicsWorld& getInstance();
	PhysicsWorld(const PhysicsWorld&) = delete;
	void operator=(const PhysicsWorld&) = delete;

	bool init();
	void registerInQueue(const PhysicsCommand& command);
	void step();

private:
	// hidden constructor
	PhysicsWorld();

	std::vector<PhysicsCommand> m_physicsQueue;
};