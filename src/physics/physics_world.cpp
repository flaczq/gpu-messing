#include "../configs/log_config.hpp"
#include "physics_world.h"

PhysicsWorld::PhysicsWorld() {
}

bool PhysicsWorld::init() {
	m_active = true;

	return true;
}

void PhysicsWorld::fixedUpdate(float fixedt) const {
	if (m_active) {
		//LOG("Physics working: " << fixedt);
	} else {
		//LOG_D("Physics is disabled");
	}
}

void PhysicsWorld::saveState() {
}

void PhysicsWorld::togglePhysics() {
	m_active = !m_active;
	LOG_D("Changed PhysicsWorld to: " << std::boolalpha << m_active);
}