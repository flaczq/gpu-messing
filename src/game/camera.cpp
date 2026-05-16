#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../cores/back_end.h"
#include "../managers/input_manager.h"
#include "camera.h"
#include <ios>
#include <iostream>
#include <string>
#include <vector>

Camera::Camera(unsigned int width, unsigned int height)
    : m_view(0.0f),
      m_viewPos(6.0f, 1.75f, 6.0f),
      m_preViewPos(6.0f, 1.75f, 6.0f),
      m_front(0.0f, 0.0f, -1.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_right(1.0f, 0.0f, 0.0f),
      // looking at (0,0,0)
      m_yaw(-135.0f),
      m_pitch(-11.5f),
      m_fov(FOV),
      m_nearPlane(NEAR_PLANE),
      m_farPlane(FAR_PLANE),
      m_aspect((float)width / (float)height)
{
    std::fill(std::begin(m_currDirections), std::end(m_currDirections), false);
}

bool Camera::init() {
    updateVectors();
    updateProjection(true);

    return true;
}

void Camera::saveState() {
    m_preViewPos = m_viewPos;
}

// continuous key clicks -> movement
void Camera::processInput() {
    processMouseScroll(InputManager::getInstance().getScrollOffset());
    processMouseMovement(InputManager::getInstance().getOffsetX(), InputManager::getInstance().getOffsetY());

    // CROUCHING/STANDING
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_C)) {
        toggleCameraMode();
    }
    // GOD MODE
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_G)) {
        toggleGodMode();
    }

    std::fill(std::begin(m_currDirections), std::end(m_currDirections), false);

    // MOVEMENT
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_W)) {
        m_currDirections[static_cast<int>(CameraDirection::FORWARD)] = true;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_S)) {
        m_currDirections[static_cast<int>(CameraDirection::BACKWARD)] = true;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_A)) {
        m_currDirections[static_cast<int>(CameraDirection::LEFT)] = true;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_D)) {
        m_currDirections[static_cast<int>(CameraDirection::RIGHT)] = true;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_E)) {
        m_currDirections[static_cast<int>(CameraDirection::UP)] = true;
    }
    if (InputManager::getInstance().isKeyDown(GLFW_KEY_Q)) {
        m_currDirections[static_cast<int>(CameraDirection::DOWN)] = true;
    }
}

void Camera::fixedUpdate(float fixedt) {
    glm::vec3 oldPos = m_viewPos;
    float velocity = MOVEMENT_SPEED * fixedt;

    if (m_currDirections[static_cast<int>(CameraDirection::FORWARD)]) {
        m_viewPos += m_front * velocity;
    }
    if (m_currDirections[static_cast<int>(CameraDirection::BACKWARD)]) {
        m_viewPos -= m_front * velocity;
    }
    if (m_currDirections[static_cast<int>(CameraDirection::LEFT)]) {
        m_viewPos -= m_right * velocity;
    }
    if (m_currDirections[static_cast<int>(CameraDirection::RIGHT)]) {
        m_viewPos += m_right * velocity;
    }
    // GOD MODE ACTIVATED
    if (m_godMode) {
        if (m_currDirections[static_cast<int>(CameraDirection::UP)]) {
            m_viewPos += WORLD_UP * velocity;
        }
        if (m_currDirections[static_cast<int>(CameraDirection::DOWN)]) {
            m_viewPos -= WORLD_UP * velocity;
        }
    } else {
        m_viewPos.y = getCameraModeHeight();
    }

    if (m_godModeChanged || m_cameraModeChanged) {
        //m_viewPos.y = getCameraModeHeight();

        m_godModeChanged = false;
        m_cameraModeChanged = false;
    }

    //TODO: collision
    //if (collision == true) {
    //    m_viewPos = oldPos;
    //}
}

void Camera::updateVectors() {
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateView(float alpha) {
    glm::vec3 interPosition = glm::mix(m_preViewPos, m_viewPos, alpha);
    // camera position, where you looking at, up vector
    m_view = glm::lookAt(interPosition, interPosition + m_front, m_up);
}

void Camera::updateProjection(bool force) {
    if (m_projectionDirty || force) {
        m_projection = glm::mat4(1.0f);
        m_projection = glm::perspective(glm::radians(m_fov), m_aspect, m_nearPlane, m_farPlane);

        m_projectionDirty = false;
    }
}

void Camera::restoreDefaultProjection() {
    m_fov = FOV;
    m_nearPlane = NEAR_PLANE;
    m_farPlane = FAR_PLANE;
    m_projectionDirty = true;
    updateProjection(true);
}

void Camera::updateAspect(int width, int height) {
    m_aspect = ((float)width / (float)height);
    m_projectionDirty = true;
}

void Camera::toggleCameraMode() {
    std::string cameraModeStr;
    if (m_cameraMode == CameraMode::STANDING) {
        m_cameraMode = CameraMode::CROUCHING;
        cameraModeStr = "CROUCHING";
    } else {
        m_cameraMode = CameraMode::STANDING;
        cameraModeStr = "STANDING";
    }
    m_cameraModeChanged = true;
    LOG_D("Changed camera mode to: " << cameraModeStr);
}

void Camera::toggleGodMode() {
    m_godMode = !m_godMode;
    m_godModeChanged = true;
    LOG_D("Changed god mode to: " << std::boolalpha << m_godMode);
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
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    // left-right
    m_yaw += xoffset * MOUSE_SENSITIVITY;
    // up-down
    m_pitch += yoffset * MOUSE_SENSITIVITY;
    if (constrainPitch) {
        if (m_pitch < MIN_PITCH) {
            m_pitch = MIN_PITCH;
        }
        if (m_pitch > MAX_PITCH) {
            m_pitch = MAX_PITCH;
        }
    }
}

float Camera::getCameraModeHeight() const {
    if (m_cameraMode == CameraMode::STANDING) {
        return 1.75f;
    }
    return 0.85f;
}