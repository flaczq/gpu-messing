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

        physics->isMoving = Utils::Math::areDifferent(transform->position, transform->prevPosition);

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
        if (!cmd.physics.isMoving) {
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
            //if (targetCmd.physics.isMoving) {
            //	// FIXME include floor: isGrounded
            //	continue;
            //}
            if (cmd.physics.layer < targetCmd.physics.layer) {
                // (p)layering
                continue;
            }

            // i want my...
            glm::vec3 mtv = _findMinimumTranslationVector(cmd.physics.AABB, targetCmd.physics.AABB);
            if (Utils::Math::isPositive(mtv)) {
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
    // will this ever happen..?
    //if (overlapY < minOverlap) {
    //    minOverlap = overlapY;
    //    normal = glm::vec3(0.0f, 1.0f, 0.0f);
    //}
    if (overlapZ < minOverlap) {
        minOverlap = overlapZ;
        normal = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    // calculate vector direction
    glm::vec3 aabbACenter = Utils::Math::calculateCenter(aabbA.worldMin, aabbA.worldMax);
    glm::vec3 aabbBCenter = Utils::Math::calculateCenter(aabbB.worldMin, aabbB.worldMax);
    glm::vec3 fromBtoA = aabbACenter - aabbBCenter;
    if (glm::dot(normal, fromBtoA) < 0.0f) {
        normal *= -1;
    }

    return minOverlap * normal;
}

void PhysicsSystem::_resolveCollisionWithMTV(PhysicsCommand& commandA, PhysicsCommand& commandB, const glm::vec3& mtv) {
    //LOG_D(commandA.identity.name << " <-> " << commandB.identity.name << ": " << Utils::Math::getVec3Values(mtv));
    // position adjustment
    assert(commandA.physics.isMoving);
    if (commandB.physics.isMoving) {
        // FIXME split based on mass
        commandA.transform.position += mtv * 0.5f;//* (masaB / (masaA + masaB))
        commandB.transform.position -= mtv * 0.5f;//* (masaA / (masaA + masaB))
    } else {
        // origin cannot be static = always moving
        commandA.transform.position += mtv;
    }

    // speed adjustment
    //float speedAdj = glm::dot(commandA.physics.speed, normal);
    //if (speedAdj < 0.0f) {
    //    // jitter safe
    //    commandA.physics.speed -= normal * speedAdj;
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