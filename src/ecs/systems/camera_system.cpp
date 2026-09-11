#include "../../configs/gl_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../managers/input_manager.h"
#include "../../utils/component_utils.hpp"
#include "../../utils/stats_constants.hpp"
#include "../components/camera_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "camera_system.h"

CameraSystem::CameraSystem() = default;

void CameraSystem::processInput(Registry& registry) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        processMouseScroll(camera, InputManager::getInstance().getScrollOffset());
        processMouseMovement(transform, InputManager::getInstance().getOffsetX(), InputManager::getInstance().getOffsetY());
    }
}

void CameraSystem::updateAspect(Registry& registry, int width, int height) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        // FIXME maybe NOT only for primary..?
        if (camera->type == CameraType::PRIMARY) {
            camera->aspect = ((float)width / (float)height);
        }
    }
}

void CameraSystem::updateView(Registry& registry, float alpha) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        // FIXME maybe NOT only for primary..?
        if (camera->type == CameraType::PRIMARY) {
            glm::vec3 interPosition = Utils::Component::calculateInterpolatedPosition(*transform, alpha);
            glm::vec3 front = Utils::Component::calculateFront(*transform);
            glm::vec3 up = Utils::Component::calculateUp(*transform);
            // FIXME standing/crouching
            interPosition.y += Constants::Stats::Camera::STANDING_OFFSET;
            // followed position, where you looking at, up vector
            camera->view = glm::lookAt(interPosition, interPosition + front, up);
        }
    }
}

void CameraSystem::updateProjection(Registry& registry) {
    for (Entity entity : registry.view<CameraComponent>()) {
        auto* camera = registry.getComponent<CameraComponent>(entity);

        camera->projection = Utils::Component::calculatePerspective(*camera);
    }
}

void CameraSystem::processMouseScroll(CameraComponent* camera, float yOffset) {
    if (yOffset != 0.0f) {
        camera->fov -= yOffset;
        if (camera->fov < Constants::Stats::Camera::MIN_FOV) {
            camera->fov = Constants::Stats::Camera::MIN_FOV;
        }
        if (camera->fov > Constants::Stats::Camera::MAX_FOV) {
            camera->fov = Constants::Stats::Camera::MAX_FOV;
        }
        //camera->lastFov = camera->fov;
    }
}

void CameraSystem::processMouseMovement(TransformComponent* transform, float xOffset, float yOffset, bool clampPitch) {
    // left-right
    transform->yaw += xOffset * Constants::Stats::Camera::MOUSE_SENSITIVITY;
    // up-down
    transform->pitch += yOffset * Constants::Stats::Camera::MOUSE_SENSITIVITY;
    if (clampPitch) {
        transform->pitch = glm::clamp(transform->pitch, Constants::Stats::Camera::MIN_PITCH, Constants::Stats::Camera::MAX_PITCH);
    }
    // update rotation
    glm::quat qYaw = glm::angleAxis(glm::radians(-transform->yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qPitch = glm::angleAxis(glm::radians(transform->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    transform->rotation = qYaw * qPitch;
    //transform->prevRotation = transform->rotation;
}