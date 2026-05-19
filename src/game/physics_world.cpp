#include "../configs/log_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/renderer.h"
#include "../managers/scene_manager.h"
#include "physics_world.h"
#include <vector>

PhysicsWorld::PhysicsWorld() {
	m_gameEntities = SceneManager::getInstance().getCurrentSceneGameEntities();
}

void PhysicsWorld::init() {
	for (auto& a : m_gameEntities) {
		LOG_D(a->getName());
	}
}