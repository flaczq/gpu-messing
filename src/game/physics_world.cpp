#include "../configs/log_config.hpp"
#include "physics_world.h"

PhysicsWorld::PhysicsWorld() {
}

void PhysicsWorld::init() {
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