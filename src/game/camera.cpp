#include "../components/transform_component.h"
#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../cores/back_end.h"
#include "../game/game_entity.h"
#include "../managers/input_manager.h"
#include "../utils/enum_utils.hpp"
#include "camera.h"
#include <ios>
#include <iostream>
#include <string>
#include <vector>

Camera::Camera(unsigned int width, unsigned int height)
    : m_view(0.0f),
      m_fov(FOV),
      m_lastFov(FOV),
      m_nearPlane(NEAR_PLANE),
      m_farPlane(FAR_PLANE),
      m_aspect((float)width / (float)height)
{
}

bool Camera::init() {
    updateProjection(true);

    return true;
}

void Camera::processInput() {
    processMouseScroll(InputManager::getInstance().getScrollOffset());
    processMouseMovement(InputManager::getInstance().getOffsetX(), InputManager::getInstance().getOffsetY());
}

void Camera::follow(TransformComponent* transform) {
    m_followedTransform = transform;
    LOG_D("Camera is following: " << transform->getOwner()->getName());

    updateProjection(true);
}

void Camera::updateView(float alpha) {
    if (m_followedTransform) {
        glm::vec3 interPosition = m_followedTransform->getInterpolatedPosition(alpha);
        // FIXME standing/crouching
        interPosition.y = STANDING_OFFSET;
        // followed position, where you looking at, up vector
        m_view = glm::lookAt(interPosition, interPosition + m_followedTransform->getFront(), m_followedTransform->getUp());
    }
}

void Camera::updateProjection(bool force) {
    if (m_projectionDirty || force) {
        m_projection = glm::mat4(1.0f);
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_nearPlane, m_farPlane);

        m_projectionDirty = false;
    }
}

void Camera::restoreDefaultProjection() {
    m_fov = m_lastFov;
    m_nearPlane = NEAR_PLANE;
    m_farPlane = FAR_PLANE;
    m_projectionDirty = true;
    updateProjection(true);
}

void Camera::updateAspect(int width, int height) {
    m_aspect = ((float)width / (float)height);
    m_projectionDirty = true;
}

void Camera::processMouseScroll(float yoffset) {
    if (yoffset != 0.0f) {
        m_projectionDirty = true;
        m_fov -= yoffset;
        if (m_fov < MIN_FOV) {
            m_fov = MIN_FOV;
        }
        if (m_fov > MAX_FOV) {
            m_fov = MAX_FOV;
        }
        m_lastFov = m_fov;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean clampPitch) {
    if (m_followedTransform) {
        // left-right
        m_followedTransform->addYaw(xoffset * MOUSE_SENSITIVITY);

        // up-down
        m_followedTransform->addPitch(yoffset * MOUSE_SENSITIVITY);
        if (clampPitch) {
            m_followedTransform->setPitch(glm::clamp(m_followedTransform->getPitch(), MIN_PITCH, MAX_PITCH));
        }
    }
}