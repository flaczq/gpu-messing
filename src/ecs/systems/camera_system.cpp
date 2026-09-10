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
        processMouseMovement(transform, camera, InputManager::getInstance().getOffsetX(), InputManager::getInstance().getOffsetY());
    }
}

void CameraSystem::updateAspect(Registry& registry, int width, int height) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        // FIXME maybe NOT only for primary..?
        if (camera->isPrimary) {
            camera->aspect = ((float)width / (float)height);
            camera->isProjectionDirty = true;
        }
    }
}

void CameraSystem::updateView(Registry& registry, float alpha) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        glm::vec3 interPosition = Utils::Component::getInterpolatedPosition(*transform, alpha);
        glm::vec3 front = Utils::Component::getFront(*transform);
        glm::vec3 up = Utils::Component::getUp(*transform);
        // FIXME standing/crouching
        interPosition.y += Constants::Stats::Camera::STANDING_OFFSET;
        // followed position, where you looking at, up vector
        camera->view = glm::lookAt(interPosition, interPosition + front, up);
    }
}

void CameraSystem::updateProjection(Registry& registry, bool force) {
    for (Entity entity : registry.view<CameraComponent>()) {
        auto* camera = registry.getComponent<CameraComponent>(entity);

        if (camera->isProjectionDirty || force) {
            camera->projection = glm::mat4(1.0f);
            camera->projection = glm::perspective(glm::radians(camera->fov), camera->aspect, camera->nearPlane, camera->farPlane);
            camera->isProjectionDirty = false;
        }
    }
}

void CameraSystem::restoreDefaultProjection(Registry& registry) {
    for (Entity entity : registry.view<CameraComponent>()) {
        auto* camera = registry.getComponent<CameraComponent>(entity);

        camera->fov = camera->lastFov;
        camera->nearPlane = Constants::Stats::Camera::NEAR_PLANE;
        camera->farPlane = Constants::Stats::Camera::FAR_PLANE;
        camera->isProjectionDirty = true;
        updateProjection(registry, true);
    }
}

void CameraSystem::processMouseScroll(CameraComponent* cameraComponent, float yOffset) {
    if (yOffset != 0.0f) {
        cameraComponent->isProjectionDirty = true;
        cameraComponent->fov -= yOffset;
        if (cameraComponent->fov < Constants::Stats::Camera::MIN_FOV) {
            cameraComponent->fov = Constants::Stats::Camera::MIN_FOV;
        }
        if (cameraComponent->fov > Constants::Stats::Camera::MAX_FOV) {
            cameraComponent->fov = Constants::Stats::Camera::MAX_FOV;
        }
        cameraComponent->lastFov = cameraComponent->fov;
    }
}

void CameraSystem::processMouseMovement(TransformComponent* transform, CameraComponent* cameraComponent, float xOffset, float yOffset, bool clampPitch) {
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