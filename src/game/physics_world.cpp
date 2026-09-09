#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../ecs/entity.hpp"
#include "../graphics/renderer.h"
#include "../managers/scene_manager.h"
#include "../utils/color_constants.hpp"
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
		glm::vec3 color = physicsBody.second.physics->isColliding() ? Constants::Color::RED : Constants::Color::GREEN;
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