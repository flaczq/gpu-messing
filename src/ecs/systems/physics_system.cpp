#include "../entites/entity.hpp"
#include "../registry.h"
#include "physics_system.h"

PhysicsSystem::PhysicsSystem() = default;

void PhysicsSystem::fixedUpdate(Registry& registry, float fixedt) {
    m_AABB.updateToWorld(m_transform->getPosition(), m_transform->getRotation(), m_transform->getScale());

    PhysicsCommandType commandType;
    if (getOwner()->isAlive() && !getOwner()->isPendingDeath()) {
        commandType = PhysicsCommandType::ADD;
    } else {
        commandType = PhysicsCommandType::REMOVE;
    }
    PhysicsComponent* physics = this;
    PhysicsBody body = {
        m_transform,
        physics
    };
    PhysicsCommand command = {
        m_transform->getOwner()->getName(),
        commandType,
        body
    };
    PhysicsWorld::getInstance().registerInQueue(command);
}