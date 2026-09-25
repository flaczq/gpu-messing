#include "../../configs/log_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../utils/math_utils.hpp"
#include "../components/identity_component.hpp"
#include "../components/physics_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "physics_system.h"
#include <limits>

PhysicsSystem::PhysicsSystem() = default;

bool PhysicsSystem::init() {
    // FIXME hardcoded max: 100
    m_physicsQueue.reserve(100);

    return true;
}

void PhysicsSystem::fixedUpdate(Registry& registry, float fixedt) {
    for (Entity entity : registry.view<IdentityComponent, TransformComponent, PhysicsComponent>()) {
        auto* identity = registry.getComponent<IdentityComponent>(entity);
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* physics = registry.getComponent<PhysicsComponent>(entity);

        _updateAABB(physics->AABB, transform->position, transform->rotation, transform->scale);

        PhysicsCommand command = {
            *identity,
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
        if (cmd.physics.isStatic) {
            // static objects can only be the target
            continue;
        }
        if (cmd.physics.layer != PhysicsLayer::TOP) {
            // ONLY TO(P)LAYER
            continue;
        }

        for (auto& targetCmd : m_physicsQueue) {
            if (&cmd.physics == &targetCmd.physics) {
                // same home address
                continue;
            }
            if (targetCmd.physics.isStatic) {
            	// FIXME include floor: isGrounded
            	continue;
            }
            if (cmd.physics.layer < targetCmd.physics.layer) {
                // (p)layering
                continue;
            }

            // i want my...
            glm::vec3 mtv = _findMinimumTranslationVector(cmd.physics.AABB, targetCmd.physics.AABB);
            if (glm::length(mtv) > 0.0f) {
                cmd.physics.isColliding = true;
                targetCmd.physics.isColliding = true;

                _resolveCollisionWithMTV(cmd, targetCmd, mtv);
                break;
            }
        }
    }

    m_physicsQueue.clear();
}

glm::vec3 PhysicsSystem::_findMinimumTranslationVector(const AABB& aabbA, const AABB& aabbB) {
    // Separating Axis Theorem
    float overlapX = glm::min(aabbA.worldMax.x, aabbB.worldMax.x) - glm::max(aabbA.worldMin.x, aabbB.worldMin.x);
    float overlapY = glm::min(aabbA.worldMax.y, aabbB.worldMax.y) - glm::max(aabbA.worldMin.y, aabbB.worldMin.y);
    float overlapZ = glm::min(aabbA.worldMax.z, aabbB.worldMax.z) - glm::max(aabbA.worldMin.z, aabbB.worldMin.z);
    // check if AABBs are overlapping at all
    if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) {
        return glm::vec3(0.0f);
    }

    // find smallest overlap
    float minOverlap = std::numeric_limits<float>::max();
    glm::vec3 normal = glm::vec3(0.0f);
    if (overlapX < minOverlap) {
        minOverlap = overlapX;
        normal = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (overlapY < minOverlap) {
        minOverlap = overlapY;
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (overlapZ < minOverlap) {
        minOverlap = overlapZ;
        normal = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    // calculate vector direction
    glm::vec3 aabbACenter = Utils::Math::calculateCenter(aabbA.worldMin, aabbA.worldMax);
    glm::vec3 aabbBCenter = Utils::Math::calculateCenter(aabbB.worldMin, aabbB.worldMax);
    glm::vec3 fromYtoX = aabbACenter - aabbBCenter;
    if (glm::dot(normal, fromYtoX) < 0.0f) {
        normal *= -1;
    }

    return minOverlap * normal;
}

void PhysicsSystem::_resolveCollisionWithMTV(PhysicsCommand& commandX, PhysicsCommand& commandY, const glm::vec3& mtv) {
    LOG_D(commandX.identity.name << " <-> " << commandY.identity.name << ": " << Utils::Math::getVec3Values(mtv));
    // position adjustment
    if (commandY.physics.isStatic) {
        // whole force used to adjust only other object
        commandX.transform.position += mtv;
    } else {
        // TODO adjust only objects that are moving
        // FIXME split based on mass
        // 50/50 adjustment
        commandX.transform.position += mtv * 0.5f;
        commandY.transform.position += mtv * 0.5f;
    }

    // speed adjustment
    // Zerujemy prędkość A skierowaną w stronę przeszkody, aby obiekt nie "drgał"
    //float speedAdj = glm::dot(commandX.physics.speed, normal);
    //if (speedAdj < 0.0f) {
    //    // jitter safe
    //    commandX.physics.speed -= normal * speedAdj;
    //}
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
    for (unsigned int i{}; i < 8; i++) {
        glm::vec3 corner = glm::vec3(model * glm::vec4(corners[i], 1.0f));
        aabb.worldMin = glm::min(aabb.worldMin, corner);
        aabb.worldMax = glm::max(aabb.worldMax, corner);
    }
}