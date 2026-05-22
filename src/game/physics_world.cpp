#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/renderer.h"
#include "../managers/scene_manager.h"
#include "physics_world.h"
#include <algorithm>
#include <string>
#include <vector>

PhysicsWorld& PhysicsWorld::getInstance() {
	static PhysicsWorld instance;
	return instance;
}

PhysicsWorld::PhysicsWorld() = default;

PhysicsWorld::~PhysicsWorld() {
	//for (auto& physicsBody : m_physicsBodies) {
	//	delete &physicsBody;
	//}

	m_physicsBodies.clear();
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
		auto it = m_physicsBodies.end();// std::find(m_physicsBodies.begin(), m_physicsBodies.end(), cmd.name);
		if (cmd.commandType == PhysicsCommandType::ADD) {
			if (it == m_physicsBodies.end()) {
				// does not exist -> add
				PhysicsBody physicsBody = {
					cmd.name,
					cmd.transform->getPosition(),
					glm::vec3(1.0f),
					glm::vec3(0.0f, 1.0f, 0.0f) // GREEN
				};
				m_physicsBodies.push_back(physicsBody);
			}
		} else if (cmd.commandType == PhysicsCommandType::REMOVE) {
			//if (it != m_physicsBodies.end()) {
			//	// does exist -> remove
			//	//delete &it;
			//	m_physicsBodies.erase(it);
			//}
		}
	}

	m_physicsQueue.clear();
}

void PhysicsWorld::step(float fixedt) const {
	for (auto& physicsBody : m_physicsBodies) {
		//LOG_D("ACTIVE PHYSICS: " << t->getOwner()->getName());
		bool collision = false;
		glm::vec3 color;
		if (collision) {
			// RED
			color = glm::vec3(1.0f, 0.0f, 0.0f);
		} else {
			// GREEN
			color = glm::vec3(0.0f, 1.0f, 0.0f);
		}
		RendererCommandDebug command = {
			physicsBody.position,
			physicsBody.size,
			physicsBody.color
		};
		Renderer::getInstance().registerInDebugQueue(command);
	}
	// 1. move entities by set velocity
	// 2. check for collisions
	// 3. if colliding move back to previous position
}