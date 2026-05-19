#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../game/game_entity.h"
#include "../managers/scene_manager.h"
#include "physics_world.h"
#include <vector>

PhysicsWorld& PhysicsWorld::getInstance() {
	static PhysicsWorld instance;
	return instance;
}

PhysicsWorld::PhysicsWorld() = default;

bool PhysicsWorld::init() {
	return true;
}

void PhysicsWorld::registerInQueue(const PhysicsCommand& command) {
	m_physicsQueue.push_back(command);
}

void PhysicsWorld::step() {
	for (auto& cmd : m_physicsQueue) {
		LOG_D(cmd.transform->getOwner()->getName() << " is solid: " << cmd.transform->getOwner()->isSolid());
	}
}