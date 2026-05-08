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

Camera::Camera(unsigned int screenWidth, unsigned int screenHeight)
    : m_view(0.0f),
      m_viewPos(6.0f, 1.75f, 6.0f),
      m_front(0.0f, 0.0f, -1.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_right(1.0f, 0.0f, 0.0f),
      // looking at (0,0,0)
      m_yaw(-135.0f),
      m_pitch(-11.5f),
      m_fov(45.0f),
      m_aspect((float)screenWidth / (float)screenHeight)
{
    std::fill(std::begin(m_currDirections), std::end(m_currDirections), false);

    updateCameraVectors();
    updateProjection();
}

bool Camera::init() {
    return true;
}

void Camera::updateAspect(int width, int height) {
    m_aspect = ((float)width / (float)height);
    m_projectionDirty = true;
}

// continuous key clicks -> movement
void Camera::update(double dt) {
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

    // no need for interpolation (camera works every frame)
    updateCameraVectors();
};

void Camera::lateUpdate(double dt) {
    if (m_currDirections[static_cast<int>(CameraDirection::FORWARD)]) {
        processKeyboard(CameraDirection::FORWARD, dt);
    }
    if (m_currDirections[static_cast<int>(CameraDirection::BACKWARD)]) {
        processKeyboard(CameraDirection::BACKWARD, dt);
    }
    if (m_currDirections[static_cast<int>(CameraDirection::LEFT)]) {
        processKeyboard(CameraDirection::LEFT, dt);
    }
    if (m_currDirections[static_cast<int>(CameraDirection::RIGHT)]) {
        processKeyboard(CameraDirection::RIGHT, dt);
    }
    if (m_currDirections[static_cast<int>(CameraDirection::UP)]) {
        processKeyboard(CameraDirection::UP, dt);
    }
    if (m_currDirections[static_cast<int>(CameraDirection::DOWN)]) {
        processKeyboard(CameraDirection::DOWN, dt);
    }

    if (m_projectionDirty) {
        updateProjection();
    }
    if (m_godModeChanged || m_cameraModeChanged) {
        m_viewPos.y = getCameraModeHeight();
    }

    // camera position, where you looking at, up vector
    m_view = glm::lookAt(m_viewPos, m_viewPos + m_front, m_up);

    m_projectionDirty = false;
    m_godModeChanged = false;
    m_cameraModeChanged = false;
};

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
};

void Camera::toggleGodMode() {
    m_godMode = !m_godMode;
    m_godModeChanged = true;
    LOG_D("Changed god mode to: " << std::boolalpha << m_godMode);
}

void Camera::processKeyboard(CameraDirection direction, double dt) {
    float velocity = MOVEMENT_SPEED * static_cast<float>(dt);
    if (direction == CameraDirection::FORWARD) {
        m_viewPos += m_front * velocity;
    }
    if (direction == CameraDirection::BACKWARD) {
        m_viewPos -= m_front * velocity;
    }
    if (direction == CameraDirection::LEFT) {
        m_viewPos -= m_right * velocity;
    }
    if (direction == CameraDirection::RIGHT) {
        m_viewPos += m_right * velocity;
    }

    // GOD MODE ACTIVATED
    if (m_godMode) {
        if (direction == CameraDirection::UP) {
            m_viewPos += m_up * velocity;
        }
        if (direction == CameraDirection::DOWN) {
            m_viewPos -= m_up * velocity;
        }
    } else {
        m_viewPos.y = getCameraModeHeight();
    }
}

void Camera::processMouseScroll(float yoffset) {
    if (yoffset != 0.0f) {
        m_projectionDirty = true;
        m_fov -= yoffset;
        if (m_fov < 1.0f) {
            m_fov = 1.0f;
        }
        if (m_fov > 90.0f) {
            m_fov = 90.0f;
        }
    }
};

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    m_yaw += xoffset * MOUSE_SENSITIVITY;
    m_pitch += yoffset * MOUSE_SENSITIVITY;
    if (constrainPitch) {
        if (m_pitch < -90.0f) {
            m_pitch = -90.0f;
        }
        if (m_pitch > 90.0f) {
            m_pitch = 90.0f;
        }
    }
}

void Camera::updateCameraVectors() {
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, WORLD_UP));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::updateProjection() {
    m_projection = glm::mat4(1.0f);
    m_projection = glm::perspective(glm::radians(m_fov), m_aspect, NEAR_PLANE, FAR_PLANE);
}

float Camera::getCameraModeHeight() const {
    if (m_cameraMode == CameraMode::STANDING) {
        return 1.75f;
    }
    return 0.85f;
}