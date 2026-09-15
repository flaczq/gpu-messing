#include "../../configs/log_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../utils/math_utils.hpp"
#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "physics_system.h"

PhysicsSystem::PhysicsSystem() = default;

bool PhysicsSystem::init() {
    // FIXME hardcoded max: 100
    m_physicsQueue.reserve(100);

    return true;
}

void PhysicsSystem::fixedUpdate(Registry& registry, float fixedt) {
    for (Entity entity : registry.view<TransformComponent, PhysicsComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* physics = registry.getComponent<PhysicsComponent>(entity);

        _updateAABB(physics->AABB, transform->position, transform->rotation, transform->scale);

        PhysicsCommand command = {
            *transform,
            *physics
        };
        _registerInQueue(command);
    }
}

void PhysicsSystem::_registerInQueue(const PhysicsCommand& command) {
    m_physicsQueue.push_back(command);
}

void PhysicsSystem::execute() {
    for (auto& cmd : m_physicsQueue) {
        cmd.physics.isColliding = false;
    }

    for (auto& cmd : m_physicsQueue) {
        if (cmd.physics.isColliding) {
            continue;
        }
        if (cmd.physics.layer != PhysicsLayer::TOP) {
            // ONLY TO(P)LAYER
            continue;
        }

        for (auto& targetCmd : m_physicsQueue) {
            if (&cmd.physics == &targetCmd.physics) {
                // home address
                continue;
            }
            //if (targetCmd.physics.isColliding) {
            //	// checked
            //	continue;
            //}
            if (cmd.physics.layer < targetCmd.physics.layer) {
                // (p)layering
                continue;
            }

            if (_detectCollision(cmd.physics, targetCmd.physics)) {
                cmd.physics.isColliding = true;
                targetCmd.physics.isColliding = true;

                _resolveCollisionByMTV(cmd, targetCmd);
                //LOG_D(&cmd << " <-> " << &targetCmd);
                break;
            }
        }
    }

    m_physicsQueue.clear();
}

bool PhysicsSystem::_detectCollision(const PhysicsComponent& physicsX, const PhysicsComponent& physicsY) {
    // FIXME first simple AABB collision check
    // later detail collision check
    bool colliding = _isCollidingByAABB(physicsX.AABB, physicsY.AABB);
    return colliding;
}

bool PhysicsSystem::_isCollidingByAABB(const AABB& aabbX, const AABB& aabbY) {
    bool collX = (aabbX.worldMin.x <= aabbY.worldMax.x) && (aabbX.worldMax.x >= aabbY.worldMin.x);
    bool collY = (aabbX.worldMin.y <= aabbY.worldMax.y) && (aabbX.worldMax.y >= aabbY.worldMin.y);
    bool collZ = (aabbX.worldMin.z <= aabbY.worldMax.z) && (aabbX.worldMax.z >= aabbY.worldMin.z);
    return collX && collY && collZ;
}

// TODO: Minimal Translation Vector
void PhysicsSystem::_resolveCollisionByMTV(PhysicsCommand& commandX, PhysicsCommand& commandY) {
    //commandX.transform.position += glm::vec3(-1.0f, 0.0f, -1.0f);
    // FIXME maybe check if it's moving..?
    // then resolve only for moving entities
    //commandY.transform.addPosition(glm::vec3(-1.0f));
}

void PhysicsSystem::_updateAABB(AABB& aabb, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
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