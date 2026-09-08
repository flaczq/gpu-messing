#include "../../configs/math_config.hpp"
#include "../components/ai_component.hpp"
#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "ai_system.h"

AISystem::AISystem() = default;

void AISystem::fixedUpdate(Registry& registry, float fixedt) {
    for (Entity entity : registry.view<TransformComponent, PhysicsComponent, AIComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* physics = registry.getComponent<PhysicsComponent>(entity);
        auto* ai = registry.getComponent<AIComponent>(entity);
        
        if (!physics->isColliding()) {
            static float tt = 0.0f;
            tt += fixedt;
            float x = sin(tt * 0.5f) * 2.0f;
            float z = cos(tt);
            glm::vec3 newPos = glm::normalize(glm::vec3(x, 0.0f, z));
            transform->setPosition(newPos);
            transform->setRotation(newPos);
        }
    }
}