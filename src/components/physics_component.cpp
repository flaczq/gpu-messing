#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../game/physics_world.h"
#include "../utils/math_utils.hpp"
#include "component.h"
#include "physics_component.h"
#include "transform_component.h"

PhysicsComponent::PhysicsComponent(glm::vec3 AABBmin, glm::vec3 AABBmax)
    : m_AABB(AABBmin, AABBmax, AABBmin, AABBmax)
{
}

void PhysicsComponent::onInit() {
    m_transform = getOwner()->getTransform();

    m_AABB.updateWorld(m_transform->getPosition(), m_transform->getRotation(), m_transform->getScale());
}

void PhysicsComponent::onFixedUpdate(float fixedt) {
    PhysicsCommandType commandType;
    if (getOwner()->isAlive() && !getOwner()->isPendingDeath()) {
        commandType = PhysicsCommandType::ADD;
    } else {
        commandType = PhysicsCommandType::REMOVE;
    }

    //if (m_transform->getOwner()->getName() == "arms") {
    //    LOG_D("localMin: " << Utils::getVec3Values(m_AABB.localMin) << "\n" <<
    //          "localMax: " << Utils::getVec3Values(m_AABB.localMax) << "\n" <<
    //          "worldMin: " << Utils::getVec3Values(m_AABB.worldMin) << "\n" <<
    //          "worldMax: " << Utils::getVec3Values(m_AABB.worldMax) << "\n");
    //}
    m_AABB.updateWorld(m_transform->getPosition(), m_transform->getRotation(), m_transform->getScale());
    PhysicsBody body = {
        m_transform,
        &m_AABB
    };
    PhysicsCommand command = {
        m_transform->getOwner()->getName(),
        commandType,
        body
    };
    PhysicsWorld::getInstance().registerInQueue(command);
}