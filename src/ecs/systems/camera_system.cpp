#include "../../configs/log_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../managers/input_manager.h"
#include "../../utils/stats_constants.hpp"
#include "../components/camera_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "camera_system.h"
#include <iomanip>

CameraSystem::CameraSystem() = default;

void CameraSystem::processInput(Registry& registry) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        _processMouseScroll(*camera, InputManager::getInstance().getScrollOffset());
        _processMouseMovement(*transform, *camera, InputManager::getInstance().getOffsetX(), InputManager::getInstance().getOffsetY());
    }
}

// unused: moved to RenderSystem
//void CameraSystem::updateView(Registry& registry, float alpha) {
//    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
//        auto* transform = registry.getComponent<TransformComponent>(entity);
//        auto* camera = registry.getComponent<CameraComponent>(entity);
//
//        glm::vec3 interPosition = Utils::Component::calculateInterpolatedPosition(*transform, alpha);
//        glm::vec3 front = Utils::Component::calculateFront(*transform);
//        glm::vec3 up = Utils::Component::calculateUp(*transform);
//        // FIXME standing/crouching
//        interPosition.y += Constants::Stats::Camera::STANDING_OFFSET;
//            
//        // followed position, where you looking at, up vector
//        camera->view = glm::lookAt(interPosition, interPosition + front, up);
//    }
//}

// unused: moved to RenderSystem
//void CameraSystem::updateProjection(Registry& registry) {
//    for (Entity entity : registry.view<CameraComponent>()) {
//        auto* camera = registry.getComponent<CameraComponent>(entity);
//        
//        camera->projection = Utils::Component::calculateProjection(camera->fov, camera->aspect, camera->nearPlane, camera->farPlane);
//    }
//}

void CameraSystem::updateAspect(Registry& registry, int width, int height) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        if (camera->isPrimary) {
            camera->aspect = (float)width / (float)height;
            // TODO only primary Camera for now
            break;
        }
    }
}

void CameraSystem::logPosition(Registry& registry) {
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        if (camera->isPrimary) {
            std::cout << std::fixed << std::setprecision(2);
            LOG("Camera: "
                << "X: "     << std::showpos << transform->position.x << "   "
                << "Y: "     << std::showpos << transform->position.y << "   "
                << "Z: "     << std::showpos << transform->position.z << "   "
                << "YAW: "   << std::showpos << camera->yaw           << "   "
                << "PITCH: " << std::showpos << camera->pitch);
            // TODO only primary Camera for now
            break;
        }
    }
}

void CameraSystem::_processMouseScroll(CameraComponent& camera, float yOffset) {
    if (yOffset != 0.0f) {
        camera.fov -= yOffset;
        if (camera.fov < Constants::Stats::Camera::MIN_FOV) {
            camera.fov = Constants::Stats::Camera::MIN_FOV;
        }
        if (camera.fov > Constants::Stats::Camera::MAX_FOV) {
            camera.fov = Constants::Stats::Camera::MAX_FOV;
        }
    }
}

void CameraSystem::_processMouseMovement(TransformComponent& transform, CameraComponent& camera, float xOffset, float yOffset, bool clampPitch) {
    // left-right
    camera.yaw += xOffset * Constants::Stats::Camera::MOUSE_SENSITIVITY;
    // up-down
    camera.pitch += yOffset * Constants::Stats::Camera::MOUSE_SENSITIVITY;
    if (clampPitch) {
        camera.pitch = glm::clamp(camera.pitch, Constants::Stats::Camera::MIN_PITCH, Constants::Stats::Camera::MAX_PITCH);
    }
    // update rotation
    glm::quat qYaw = glm::angleAxis(glm::radians(-camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat qPitch = glm::angleAxis(glm::radians(camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    transform.rotation = qYaw * qPitch;
    //transform->prevRotation = transform->rotation;
}