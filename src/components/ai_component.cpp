#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "ai_component.h"
#include "component.h"

void AIComponent::onInit() {
    m_transform = getOwner()->getTransform();
}

void AIComponent::onFixedUpdate(float fixedt) {
    static float tt = 0.0f;
    tt += fixedt;
    float x = sin(tt * 0.5f) * 2.0f;
    float z = cos(tt);
    glm::vec3 newPos = glm::normalize(glm::vec3(x, 0.0f, z));
    m_transform->setPosition(newPos);
}