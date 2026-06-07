#pragma once

#include "../configs/math_config.hpp"
#include "../graphics/renderer.h"
#include <vector>

class TransformComponent;

enum class PhysicsCommandType {
	ADD,
	REMOVE
};

struct PhysicsCommand {
	PhysicsCommandType commandType;
	TransformComponent* transform;
};

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
	void step(float fixedt) const;

	std::vector<RendererImmediateCommand> getAABBCommand();

private:
	// hidden constructor
	PhysicsWorld();

	std::vector<PhysicsCommand> m_physicsQueue;
	std::vector<TransformComponent*> m_physicsBodies;

	unsigned int m_AABBVAO{}, m_AABBVBO{};
};