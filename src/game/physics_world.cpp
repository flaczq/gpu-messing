#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../managers/scene_manager.h"
#include "physics_world.h"
#include <algorithm>
#include <vector>

PhysicsWorld& PhysicsWorld::getInstance() {
	static PhysicsWorld instance;
	return instance;
}

PhysicsWorld::PhysicsWorld() = default;

PhysicsWorld::~PhysicsWorld() {
	for (auto* ap : m_activePhysics) {
		delete ap;
	}

	m_activePhysics.clear();
}

bool PhysicsWorld::init() {
	// FIXME hardcoded max: 100
	m_physicsQueue.reserve(100);
	return true;
}

void PhysicsWorld::registerInQueue(const PhysicsCommand& command) {
	m_physicsQueue.push_back(command);
}

void PhysicsWorld::flush() {
	for (auto& cmd : m_physicsQueue) {
		auto it = std::find(m_activePhysics.begin(), m_activePhysics.end(), cmd.transform);
		if (cmd.commandType == PhysicsCommandType::ADD) {
			if (it == m_activePhysics.end()) {
				// does not exist -> add
				m_activePhysics.push_back(cmd.transform);
			}
		} else if (cmd.commandType == PhysicsCommandType::REMOVE) {
			if (it != m_activePhysics.end()) {
				// does exist -> remove
				delete *it;
				m_activePhysics.erase(it);
			}
		}
	}

	m_physicsQueue.clear();
}

void PhysicsWorld::step(float fixedt) const {
	for (auto* t : m_activePhysics) {
		LOG_D("ACTIVE PHYSICS: " << t->getOwner()->getName());
	}
	// 1. move entities by set velocity
	// 2. check for collisions
	// 3. if colliding move back to previous position
}