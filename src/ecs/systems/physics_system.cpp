#include "../../configs/gl_config.hpp"
#include "../../configs/log_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../utils/color_constants.hpp"
#include "../../utils/component_utils.hpp"
#include "../components/physics_component.hpp"
#include "../components/render_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "physics_system.h"
#include <vector>

PhysicsSystem::PhysicsSystem() = default;

PhysicsSystem::~PhysicsSystem() {
    end();

    glDeleteVertexArrays(1, &m_VAOAABB);
    glDeleteBuffers(1, &m_VBOAABB);
}

bool PhysicsSystem::init() {
    // FIXME hardcoded max: 100
    m_physicsQueue.reserve(100);

    // hardcoded AABB 1x1x1 (with the middle at 0.0)
    float verticesAABB[] = {
        // front
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        // back
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        // connectors
        -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };
    glGenVertexArrays(1, &m_VAOAABB);
    glGenBuffers(1, &m_VBOAABB);
    glBindVertexArray(m_VAOAABB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOAABB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAABB), verticesAABB, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return true;
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

void PhysicsSystem::registerInQueue(const PhysicsCommand& command) {
    m_physicsQueue.push_back(command);
}

void PhysicsSystem::execute() {
    for (auto& cmd : m_physicsQueue) {
        cmd.physics->isColliding = false;
    }

    for (auto& cmd : m_physicsQueue) {
        if (cmd.physics->isColliding) {
            continue;
        }
        if (cmd.physics->layer != PhysicsLayer::TOP) {
            // ONLY TO(P)LAYER
            continue;
        }

        for (auto& targetCmd : m_physicsQueue) {
            if (&cmd.physics == &targetCmd.physics) {
                // home address
                continue;
            }
            //if (targetCmd.physics->isColliding) {
            //	// checked
            //	continue;
            //}
            if (cmd.physics->layer < targetCmd.physics->layer) {
                // (p)layering
                continue;
            }

            if (detectCollision(cmd.physics, targetCmd.physics)) {
                cmd.physics->isColliding = true;
                targetCmd.physics->isColliding = true;

                resolveCollisionByMTV(cmd, targetCmd);
                LOG_D(&cmd << " <-> " << &targetCmd);
                break;
            }
        }
    }

    m_physicsQueue.clear();
}

bool PhysicsSystem::detectCollision(PhysicsComponent* origin, PhysicsComponent* target) {
    // FIXME first simple AABB collision check
    // later detail collision check
    bool colliding = isCollidingByAABB(origin->AABB, target->AABB);
    return colliding;
}

bool PhysicsSystem::isCollidingByAABB(AABB origin, AABB target) {
    bool collX = (origin.worldMin.x <= target.worldMax.x) && (origin.worldMax.x >= target.worldMin.x);
    bool collY = (origin.worldMin.y <= target.worldMax.y) && (origin.worldMax.y >= target.worldMin.y);
    bool collZ = (origin.worldMin.z <= target.worldMax.z) && (origin.worldMax.z >= target.worldMin.z);
    return collX && collY && collZ;
}

// TODO: Minimal Translation Vector
void PhysicsSystem::resolveCollisionByMTV(PhysicsCommand origin, PhysicsCommand target) {
    origin.transform->position += glm::vec3(-1.0f, 0.0f, -1.0f);
    // FIXME maybe check if it's moving..?
    // then resolve only for moving entities
    //target.transform->addPosition(glm::vec3(-1.0f));
}

void PhysicsSystem::end() {
    m_physicsQueue.clear();
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

std::vector<RenderImmediateCommand> PhysicsSystem::getAABBCommand() {
    std::vector<RenderImmediateCommand> commands;
    for (auto& cmd : m_physicsQueue) {
        glm::vec3 color = cmd.physics->isColliding ? Constants::Color::RED : Constants::Color::GREEN;
        RenderImmediateCommand command = {
            m_VAOAABB,
            cmd.transform->position,
            cmd.transform->rotation,
            cmd.transform->scale,
            Utils::Component::calculateAABBSize(cmd.physics->AABB),
            Utils::Component::calculateAABBCenter(cmd.physics->AABB),
            color
        };
        commands.push_back(command);
    }
    return commands;
}