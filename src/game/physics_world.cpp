#include "../components/physics_component.h"
#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/renderer.h"
#include "../managers/scene_manager.h"
#include "../utils/colors_constants.hpp"
#include "../utils/math_utils.hpp"
#include "physics_world.h"
#include <algorithm>
#include <unordered_map>
#include <vector>

PhysicsWorld& PhysicsWorld::getInstance() {
	static PhysicsWorld instance;
	return instance;
}

PhysicsWorld::PhysicsWorld() = default;

PhysicsWorld::~PhysicsWorld() {
	end();

	glDeleteVertexArrays(1, &m_VAOAABB);
	glDeleteBuffers(1, &m_VBOAABB);
}

bool PhysicsWorld::init() {
	// FIXME hardcoded max: 100
	m_physicsQueue.reserve(100);

	// hardcoded AABB 1x1x1 (with the middle at 0.0)
	float verticesAABB[] = {
		// front
		-0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
		// back
		-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
		// connectors
		-0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
	};
	glGenVertexArrays(1, &m_VAOAABB);
	glGenBuffers(1, &m_VBOAABB);
	glBindVertexArray(m_VAOAABB);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBOAABB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAABB), verticesAABB, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return true;
}

void PhysicsWorld::registerInQueue(const PhysicsCommand& command) {
	m_physicsQueue.push_back(command);
}

void PhysicsWorld::flush() {
	//m_physicsBodies.clear();

	// add/remove
	for (auto& cmd : m_physicsQueue) {
		auto it = m_physicsBodies.find(cmd.name);
		if (cmd.commandType == PhysicsCommandType::ADD) {
			if (it == m_physicsBodies.end()) {
				// does not exist -> add
				m_physicsBodies.try_emplace(cmd.name, cmd.physicsBody.transform, cmd.physicsBody.AABB);
			}
		} else if (cmd.commandType == PhysicsCommandType::REMOVE) {
			if (it != m_physicsBodies.end()) {
				// does exist -> remove
				delete it->second.transform;
				m_physicsBodies.erase(it);
			}
		}
		//m_physicsBodies.try_emplace(cmd.name, cmd.physicsBody->transform, cmd.physicsBody->AABB);
	}

	m_physicsQueue.clear();
}

void PhysicsWorld::step(float fixedt) {
	// 1. move entities by set velocity
	// 2. check for collisions
	// 3. if colliding move back to previous position
	for (auto& physicsBody : m_physicsBodies) {
		auto it = std::find(m_collidedBodies.begin(), m_collidedBodies.end(), physicsBody.first);
		if (it != m_collidedBodies.end()) {
			// already collided
			continue;
		}

		physicsBody.second.AABB->setColor(Constants::Colors::GREEN);
		// TEST only fps arms
		if (physicsBody.first != "arms") {
			continue;
		}

		bool collision = false;
		for (auto& targetPhysicsBody : m_physicsBodies) {
			targetPhysicsBody.second.AABB->setColor(Constants::Colors::GREEN);
			// break here to set colors at the end of the loop
			if (collision) {
				break;
			}
			// itself
			if (physicsBody.first == targetPhysicsBody.first) {
				continue;
			}

			if (physicsBody.second.AABB->isInCollisionWithOther(*targetPhysicsBody.second.AABB)) {
				LOG_D(physicsBody.first << " <-> " << targetPhysicsBody.first);
				m_collidedBodies.push_back(physicsBody.first);
				m_collidedBodies.push_back(targetPhysicsBody.first);
				collision = true;
				//break;
			}

			// do NOT break before setting colors
			glm::vec3 color = collision ? Constants::Colors::RED : Constants::Colors::GREEN;
			physicsBody.second.AABB->setColor(color);
			targetPhysicsBody.second.AABB->setColor(color);
		}
	}

	m_collidedBodies.clear();
}

void PhysicsWorld::end() {
	// do NOT do this - it's taken care of elsewhere
	//for (auto& physicsBody : m_physicsBodies) {
	//	delete physicsBody.second.transform;
	//}

	m_physicsBodies.clear();
	m_collidedBodies.clear();
}

std::vector<RendererImmediateCommand> PhysicsWorld::getAABBCommand() {
	std::vector<RendererImmediateCommand> commands;
	for (auto& physicsBody : m_physicsBodies) {
		RendererImmediateCommand command = {
			m_VAOAABB,
			physicsBody.second.transform->getPosition(),
			physicsBody.second.transform->getRotation(),
			physicsBody.second.transform->getScale(),
			physicsBody.second.AABB->getSize(),
			physicsBody.second.AABB->getCenter(),
			physicsBody.second.AABB->getColor()
		};
		commands.push_back(command);
	}
	return commands;
}