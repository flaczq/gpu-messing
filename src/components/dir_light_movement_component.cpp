#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/renderer.h"
#include "component.h"
#include "dir_light_movement_component.h"

void DirLightMovementComponent::onInit() {
    m_transform = getOwner()->getTransform();
}

void DirLightMovementComponent::onFixedUpdate(float fixedt) {
    static float tt = 0.0f;
    tt += fixedt;
    float x = sin(tt);
    float z = cos(tt);
    glm::vec3 lightPos = glm::normalize(glm::vec3(x, 1.0f, z));
    m_transform->setPosition(lightPos * 11.0f);
    Renderer::getInstance().setLightDir(-lightPos);
}