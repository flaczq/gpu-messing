#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../game/physics_world.h"
#include "../utils/colors_constants.hpp"
#include "../utils/math_utils.hpp"
#include "component.h"
#include "physics_component.h"
#include "transform_component.h"

PhysicsComponent::PhysicsComponent(const glm::vec3& AABBmin, const glm::vec3& AABBmax, PhysicsLayer layer)
    : m_AABB(AABBmin, AABBmax, AABBmin, AABBmax),
      m_layer(layer)
{
}

void PhysicsComponent::onInit() {
    m_transform = getOwner()->getTransform();
    m_colliding = false;

    m_AABB.init();
    m_AABB.updateToWorld(m_transform->getPosition(), m_transform->getRotation(), m_transform->getScale());
}

void PhysicsComponent::onFixedUpdate(float fixedt) {
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

void PhysicsComponent::processCollision(bool colliding) {
    m_colliding = colliding;
    m_AABB.m_color = m_colliding ? Constants::Colors::RED : Constants::Colors::GREEN;
}