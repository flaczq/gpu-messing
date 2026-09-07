#include "../components/physics_component.h"
#include "../components/transform_component.h"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../ecs/entity.hpp"
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
				m_physicsBodies.try_emplace(cmd.name, cmd.physicsBody.transform, cmd.physicsBody.physics);
			}
		} else if (cmd.commandType == PhysicsCommandType::REMOVE) {
			if (it != m_physicsBodies.end()) {
				// does exist -> remove
				//delete it->second.transform;
				m_physicsBodies.erase(it);
			}
		}
	}

	m_physicsQueue.clear();
}

void PhysicsWorld::step(float fixedt) {
	for (auto& [name, physicsBody] : m_physicsBodies) {
		physicsBody.physics->setColliding(false);
	}

	for (auto& [name, physicsBody] : m_physicsBodies) {
		if (physicsBody.physics->isColliding()) {
			continue;
		}
		if (physicsBody.physics->getLayer() != PhysicsLayer::TOP) {
			// ONLY TO(P)LAYER
			continue;
		}

		for (auto& [targetName, targetPhysicsBody] : m_physicsBodies) {
			if (&physicsBody == &targetPhysicsBody) {
				// home address
				continue;
			}
			//if (targetPhysicsBody.physics->isColliding()) {
			//	// checked
			//	continue;
			//}
			if (physicsBody.physics->getLayer() < targetPhysicsBody.physics->getLayer()) {
				// (p)layering
				continue;
			}

			if (detectCollision(physicsBody.physics, targetPhysicsBody.physics)) {
				physicsBody.physics->setColliding(true);
				targetPhysicsBody.physics->setColliding(true);

				resolveCollisionByMTV(physicsBody, targetPhysicsBody);
				LOG_D(name << " <-> " << targetName);
				break;
			}
		}
	}
}

bool PhysicsWorld::isCollidingByAABB(AABB origin, AABB target) {
	bool collX = (origin.worldMin.x <= target.worldMax.x) && (origin.worldMax.x >= target.worldMin.x);
	bool collY = (origin.worldMin.y <= target.worldMax.y) && (origin.worldMax.y >= target.worldMin.y);
	bool collZ = (origin.worldMin.z <= target.worldMax.z) && (origin.worldMax.z >= target.worldMin.z);
	return collX && collY && collZ;
};

bool PhysicsWorld::detectCollision(PhysicsComponent* origin, PhysicsComponent* target) {
	// FIXME first simple AABB collision check
	// later detail collision check
	bool colliding = isCollidingByAABB(origin->getAABB(), target->getAABB());
	return colliding;
}

// TODO: Minimal Translation Vector
void PhysicsWorld::resolveCollisionByMTV(PhysicsBody origin, PhysicsBody target) {
	origin.transform->addPosition(glm::vec3(-1.0f, 0.0f, -1.0f));
	// FIXME maybe check if it's moving..?
	// then resolve only for moving entities
	//target.transform->addPosition(glm::vec3(-1.0f));
}

void PhysicsWorld::end() {
	// do NOT do this - it's taken care of elsewhere
	//for (auto& physicsBody : m_physicsBodies) {
	//	delete physicsBody.second.transform;
	//}

	m_physicsBodies.clear();
}

std::vector<RendererImmediateCommand> PhysicsWorld::getAABBCommand() {
	std::vector<RendererImmediateCommand> commands;
	for (auto& physicsBody : m_physicsBodies) {
		glm::vec3 color = physicsBody.second.physics->isColliding() ? Constants::Colors::RED : Constants::Colors::GREEN;
		RendererImmediateCommand command = {
			m_VAOAABB,
			physicsBody.second.transform->getPosition(),
			physicsBody.second.transform->getRotation(),
			physicsBody.second.transform->getScale(),
			physicsBody.second.physics->getAABB().getSize(),
			physicsBody.second.physics->getAABB().getCenter(),
			color
		};
		commands.push_back(command);
	}
	return commands;
}