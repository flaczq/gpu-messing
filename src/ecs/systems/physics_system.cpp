#include "../../configs/math_config.hpp"
#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "physics_system.h"
#include <vector>

PhysicsSystem::PhysicsSystem() = default;

void PhysicsSystem::init() {
    // FIXME hardcoded max: 100
    m_physicsQueue.reserve(100);
}

void PhysicsSystem::fixedUpdate(Registry& registry, float fixedt) {
    for (Entity entity : registry.view<TransformComponent, PhysicsComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* physics = registry.getComponent<PhysicsComponent>(entity);

        updateAABB(physics->AABB, transform->position, transform->rotation, transform->scale);

        PhysicsCommand command = {
            transform,
            physics
        };
        registerInQueue(command);
    }
}

void PhysicsSystem::updateAABB(AABB aabb, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model *= glm::mat4_cast(rotation);
    model = glm::scale(model, scale);

    glm::vec3 corners[8] = {
        { aabb.localMin.x, aabb.localMin.y, aabb.localMin.z },
        { aabb.localMax.x, aabb.localMin.y, aabb.localMin.z },
        { aabb.localMin.x, aabb.localMax.y, aabb.localMin.z },
        { aabb.localMax.x, aabb.localMax.y, aabb.localMin.z },
        { aabb.localMin.x, aabb.localMin.y, aabb.localMax.z },
        { aabb.localMax.x, aabb.localMin.y, aabb.localMax.z },
        { aabb.localMin.x, aabb.localMax.y, aabb.localMax.z },
        { aabb.localMax.x, aabb.localMax.y, aabb.localMax.z }
    };

    aabb.worldMin = glm::vec3(std::numeric_limits<float>::max());
    aabb.worldMax = glm::vec3(std::numeric_limits<float>::lowest());
    for (size_t i{}; i < 8; i++) {
        glm::vec3 corner = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        aabb.worldMin = glm::min(aabb.worldMin, corner);
        aabb.worldMax = glm::max(aabb.worldMax, corner);
    }
}

void PhysicsSystem::registerInQueue(const PhysicsCommand& command) {
    m_physicsQueue.push_back(command);
}