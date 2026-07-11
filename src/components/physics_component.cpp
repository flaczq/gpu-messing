#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../game/physics_world.h"
#include "component.h"
#include "physics_component.h"
#include "transform_component.h"

PhysicsComponent::PhysicsComponent(glm::vec3 AABBmin, glm::vec3 AABBmax)
    : m_AABB(AABBmin, AABBmax)
{
}

void PhysicsComponent::onInit() {
    m_transform = getOwner()->getTransform();
}

void PhysicsComponent::onFixedUpdate(float fixedt) {
    PhysicsCommandType commandType;
    if (getOwner()->isAlive() && !getOwner()->isPendingDeath()) {
        commandType = PhysicsCommandType::ADD;
    } else {
        commandType = PhysicsCommandType::REMOVE;
    }
    PhysicsBody body = {
        m_transform,
        m_AABB
    };
    PhysicsCommand command = {
        m_transform->getOwner()->getName(),
        commandType,
        &body
    };
    PhysicsWorld::getInstance().registerInQueue(command);
}