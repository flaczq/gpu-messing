#include "../../configs/math_config.hpp"
#include "../../utils/color_constants.hpp"
#include "../components/dir_light_movement_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "dir_light_movement_system.h"

DirLightMovementSystem::DirLightMovementSystem() = default;

void DirLightMovementSystem::fixedUpdate(Registry& registry, float fixedt) {
    for (Entity entity : registry.view<TransformComponent, DirLightMovementComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* dirLightMovement = registry.getComponent<DirLightMovementComponent>(entity);

        static float tt = 0.0f;
        tt += fixedt;
        float x = sin(tt * 0.5f) * 2.0f;
        float z = cos(tt);
        glm::vec3 lightPos = glm::normalize(glm::vec3(x, 0.0f, z));
        transform->position = lightPos * 11.0f;
        dirLightMovement->direction = -lightPos;
        dirLightMovement->color = Constants::Color::WHITE;
        // TODO more than single directional light
        break;
    }
}