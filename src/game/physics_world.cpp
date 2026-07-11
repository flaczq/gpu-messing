#include "../components/physics_component.h"
#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/renderer.h"
#include "../managers/scene_manager.h"
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
	for (auto& physicsBody : m_physicsBodies) {
		delete physicsBody.second.transform;
	}

	m_physicsBodies.clear();

	glDeleteVertexArrays(1, &m_VAOAABB);
	glDeleteBuffers(1, &m_VBOAABB);
}

bool PhysicsWorld::init() {
	// FIXME hardcoded max: 100
	m_physicsQueue.reserve(100);

	// hardcoded AABB
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
				m_physicsBodies.try_emplace(cmd.name, cmd.physicsBody->transform, cmd.physicsBody->AABB);
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

void PhysicsWorld::step(float fixedt) const {
	for (auto& physicsBody : m_physicsBodies) {
		//LOG_D("ACTIVE PHYSICS: " << t->getOwner()->getName());
		bool collision = false;
		glm::vec3 color;
		for (auto& targetPhysicsBody : m_physicsBodies) {
			if (physicsBody.first == targetPhysicsBody.first) {
				continue;
			}
			// FIXME local -> world
			if (physicsBody.second.AABB.isInCollisionWithOther(targetPhysicsBody.second.AABB)) {
				collision = true;
				LOG_D(physicsBody.first << " <-> " << targetPhysicsBody.first);
				break;
			}
		}
		if (collision) {
			// RED
			color = glm::vec3(1.0f, 0.0f, 0.0f);
		} else {
			// GREEN
			color = glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}
	// 1. move entities by set velocity
	// 2. check for collisions
	// 3. if colliding move back to previous position
}


std::vector<RendererImmediateCommand> PhysicsWorld::getAABBCommand() {
	std::vector<RendererImmediateCommand> commands;
	for (auto& physicsBody : m_physicsBodies) {
		glm::vec3 pos = physicsBody.second.transform->getPosition();
		RendererImmediateCommand command = {
			m_VAOAABB,
			pos,
			physicsBody.second.transform->getRotation(),
			physicsBody.second.transform->getScale(),
			physicsBody.second.AABB.getSize(),
			physicsBody.second.AABB.getCenter(),
			glm::vec3(1.0f, 0.0f, 1.0f)
		};
		commands.push_back(command);
	}
	return commands;
}