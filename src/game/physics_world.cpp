#include "../components/physics_component.h"
#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/renderer.h"
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
	for (auto& physicsBody : m_physicsBodies) {
		delete physicsBody.transform;
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
	m_physicsBodies.clear();
	for (auto& cmd : m_physicsQueue) {
		/*auto it = std::find(m_physicsBodies.begin(), m_physicsBodies.end(), cmd.transform);
		if (cmd.commandType == PhysicsCommandType::ADD) {
			if (it == m_physicsBodies.end()) {
				// does not exist -> add
				m_physicsBodies.push_back({
					cmd.transform,
					cmd.AABB
				});
			}
		} else if (cmd.commandType == PhysicsCommandType::REMOVE) {
			if (it != m_physicsBodies.end()) {
				// does exist -> remove
				delete it->transform;
				m_physicsBodies.erase(it);
			}
		}*/
		m_physicsBodies.push_back({
			cmd.physicsBody.transform,
			cmd.physicsBody.AABB
		});
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
	}
	// 1. move entities by set velocity
	// 2. check for collisions
	// 3. if colliding move back to previous position
}


std::vector<RendererImmediateCommand> PhysicsWorld::getAABBCommand() {
	std::vector<RendererImmediateCommand> commands;
	for (auto& physicsBody : m_physicsBodies) {
		RendererImmediateCommand command = {
			m_VAOAABB,
			physicsBody.transform->getPosition(),
			physicsBody.transform->getRotation(),
			physicsBody.transform->getScale(),
			physicsBody.AABB.getSize(),
			physicsBody.AABB.getCenter(),
			glm::vec3(1.0f, 0.0f, 1.0f)
		};
		commands.push_back(command);
	}
	return commands;
}