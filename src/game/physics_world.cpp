#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
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

void PhysicsWorld::saveState() {}

void PhysicsWorld::registerInQueue(const PhysicsCommand& command) {
	m_physicsQueue.push_back(command);
}

void PhysicsWorld::step(float fixedt) const {
	static float tt = 0.0f;
	tt += fixedt;
	float x = sin(tt);
	for (auto& cmd : m_physicsQueue) {
		if (cmd.transform->getOwner()->getName().starts_with("window")) {
			cmd.transform->setScale(glm::vec3(x));
		}
	}
}