#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../game/physics_world.h"
#include "component.h"
#include "physics_component.h"
#include "transform_component.h"

PhysicsComponent::PhysicsComponent() {
}

void PhysicsComponent::onInit() {
    // create bb
    m_transform = getOwner()->getTransform();

    //FIXME
    aabb.min = glm::vec3(1.0f);
    aabb.max = glm::vec3(3.0f);
}

void PhysicsComponent::onFixedUpdate(float fixedt) {
    PhysicsCommandType commandType;
    if (getOwner()->isAlive() && !getOwner()->isPendingDeath()) {
        commandType = PhysicsCommandType::ADD;
    } else {
        commandType = PhysicsCommandType::REMOVE;
    }
    PhysicsCommand command = {
        getOwner()->getName(),
        commandType,
        m_transform
    };
    PhysicsWorld::getInstance().registerInQueue(command);
}