#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "component.h"
#include "physics_component.h"
#include "transform_component.h"

void PhysicsComponent::onInit() {
    m_transform = getOwner()->getTransform();
}

void PhysicsComponent::onFixedUpdate(float fixedt) {
    // create bb or in init
}