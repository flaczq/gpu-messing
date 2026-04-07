#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../cores/back_end.h"
#include "../managers/input_manager.h"
#include "camera.h"
#include <ios>
#include <iostream>
#include <string>

Camera::Camera(GLFWwindow* window, unsigned int screenWidth, unsigned int screenHeight)
    : m_window(window),
      m_view(0.0f),
      m_position(6.0f, 1.75f, 6.0f),
      m_front(0.0f, 0.0f, -1.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_right(1.0f, 0.0f, 0.0f),
      // looking at (0,0,0)
      m_yaw(-135.0f),
      m_pitch(-11.5f),
      m_fov(45.0f),
      m_aspect((float)screenWidth / (float)screenHeight)
{
    updateCameraVectors();
}

bool Camera::init() {
    return true;
}

// continuous key clicks -> movement
void Camera::update(double dt) {
    auto& input = InputManager::getInstance();

    processMouseScroll(input.getScrollOffset());
    processMouseMovement(input.getOffsetX(), input.getOffsetY());

    // CROUCHING/STANDING
    if (input.isKeyPressed(GLFW_KEY_C)) {
        toggleCameraMode();
    }

    // GOD MODE
    if (input.isKeyPressed(GLFW_KEY_G)) {
        toggleGodMode();
    }

    // MOVEMENT
    if (input.isKeyDown(GLFW_KEY_W)) {
        processKeyboard(CameraDirection::FORWARD, dt);
    }
    if (input.isKeyDown(GLFW_KEY_S)) {
        processKeyboard(CameraDirection::BACKWARD, dt);
    }
    if (input.isKeyDown(GLFW_KEY_A)) {
        processKeyboard(CameraDirection::LEFT, dt);
    }
    if (input.isKeyDown(GLFW_KEY_D)) {
        processKeyboard(CameraDirection::RIGHT, dt);
    }
    if (input.isKeyDown(GLFW_KEY_E)) {
        processKeyboard(CameraDirection::UP, dt);
    }
    if (input.isKeyDown(GLFW_KEY_Q)) {
        processKeyboard(CameraDirection::DOWN, dt);
    }
};

void Camera::lateUpdate() {
    // no need for interpolation (camera works every frame)
    updateCameraVectors();

    // camera position, where you looking at, up vector
    m_view = glm::lookAt(m_position, m_position + m_front, m_up);
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
    m_position.y = getCameraModeHeight();
    LOG_D("Changed camera mode to: " << cameraModeStr);
};

void Camera::toggleGodMode() {
    m_godMode = !m_godMode;
    m_position.y = getCameraModeHeight();
    LOG_D("Changed god mode to: " << std::boolalpha << m_godMode);
}

void Camera::processKeyboard(CameraDirection direction, double dt) {
    float velocity = MOVEMENT_SPEED * static_cast<float>(dt);
    if (direction == CameraDirection::FORWARD) {
        m_position += m_front * velocity;
    }
    if (direction == CameraDirection::BACKWARD) {
        m_position -= m_front * velocity;
    }
    if (direction == CameraDirection::LEFT) {
        m_position -= m_right * velocity;
    }
    if (direction == CameraDirection::RIGHT) {
        m_position += m_right * velocity;
    }

    // GOD MODE ACTIVATED
    if (m_godMode) {
        if (direction == CameraDirection::UP) {
            m_position += m_up * velocity;
        }
        if (direction == CameraDirection::DOWN) {
            m_position -= m_up * velocity;
        }
    } else {
        // FPS stay on the ground, peasant
        m_position.y = getCameraModeHeight();
    }
}

void Camera::processMouseScroll(float yoffset) {
    m_fov -= yoffset;
    if (m_fov < 1.0f) {
        m_fov = 1.0f;
    }
    if (m_fov > 90.0f) {
        m_fov = 90.0f;
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

float Camera::getCameraModeHeight() const {
    if (m_cameraMode == CameraMode::STANDING) {
        return 1.75f;
    }
    return 0.85f;
}