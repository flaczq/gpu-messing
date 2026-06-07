#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../game/physics_world.h"
#include "component.h"
#include "physics_component.h"
#include "render_component.h"
#include "transform_component.h"

PhysicsComponent::PhysicsComponent() {
}

void PhysicsComponent::onInit() {
    // create bounding box
    m_transform = getOwner()->getTransform();
    m_render = getOwner()->getRender();

    m_AABB.min = m_render->getModel()->getAABBMin();
    m_AABB.max = m_render->getModel()->getAABBMax();
}

void PhysicsComponent::onFixedUpdate(float fixedt) {
    PhysicsCommandType commandType;
    if (getOwner()->isAlive() && !getOwner()->isPendingDeath()) {
        commandType = PhysicsCommandType::ADD;
    } else {
        commandType = PhysicsCommandType::REMOVE;
    }
    PhysicsCommand command = {
        commandType,
        m_transform
    };
    PhysicsWorld::getInstance().registerInQueue(command);
}