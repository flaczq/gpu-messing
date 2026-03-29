#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "back_end.h"
#include "camera.h"
#include <ios>
#include <iostream>
#include <string>

Camera::Camera(GLFWwindow* window, unsigned int screenWidth, unsigned int screenHeight)
    : m_window(window),
      m_position(6.0f, 1.0f, 6.0f),
      m_front(0.0f, 0.0f, -1.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_right(1.0f, 0.0f, 0.0f),
      m_worldUp(0.0f, 1.0f, 0.0f),
      m_orientation(1.0f, 0.0f, 0.0f, 0.0f),
      // looking at (0,0,0)
      m_yaw(-135.0f),
      m_pitch(-11.5f),
      m_firstMouse(true),
      m_lastX(screenWidth / 2.0f),
      m_lastY(screenHeight / 2.0f),
      m_movementSpeed(5.0f),
      m_mouseSensitivity(0.05f),
      m_fov(45.0f),
      m_aspect((float)screenWidth / (float)screenHeight)
{
    // CameraMode::STANDING
    m_position.y = getCameraModeHeight();
    updateCameraVectors();
}

bool Camera::init() {
    // set callbacks: mouse, scroll
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

// continuous key clicks -> movement
void Camera::processInput(float dt) {
    // EXIT
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_window, true);
    }

    // MOVEMENT
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
        processKeyboard(CameraDirection::FORWARD, dt);
    }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
        processKeyboard(CameraDirection::BACKWARD, dt);
    }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
        processKeyboard(CameraDirection::LEFT, dt);
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
        processKeyboard(CameraDirection::RIGHT, dt);
    }
    if (glfwGetKey(m_window, GLFW_KEY_E) == GLFW_PRESS) {
        processKeyboard(CameraDirection::UP, dt);
    }
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS) {
        processKeyboard(CameraDirection::DOWN, dt);
    }
};

void Camera::processKeyboard(CameraDirection direction, float dt) {
    float velocity = m_movementSpeed * dt;
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

void Camera::processMouseScroll(float yOffset) {
    m_fov -= (float)yOffset;
    if (m_fov < 1.0f) {
        m_fov = 1.0f;
    }
    if (m_fov > 90.0f) {
        m_fov = 90.0f;
    }
};

void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;

    m_yaw += xOffset;
    m_pitch += yOffset;

    if (constrainPitch) {
        if (m_pitch < -90.0f) {
            m_pitch = -90.0f;
        }
        if (m_pitch > 90.0f) {
            m_pitch = 90.0f;
        }
    }

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    // camera position, where you looking at, up vector
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

glm::vec3 Camera::getPosition() const {
    return m_position;
}

float Camera::getFov() const {
    return m_fov;
}

float Camera::getAspect() const {
    return m_aspect;
}

float Camera::getCameraModeHeight() const {
    if (m_cameraMode == CameraMode::STANDING) {
        return 1.75f;
    }
    return 0.85f;
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
    m_position.y = getCameraModeHeight();
    LOG_D("Changed camera mode to: " << cameraModeStr);
};

void Camera::toggleGodMode() {
    m_godMode = !m_godMode;
    LOG_D("Changed god mode to: " << std::boolalpha << m_godMode);
}

void Camera::updateCameraVectors() {
    glm::vec3 currentFront = glm::vec3(0.0f, 0.0f, 0.0f);
    currentFront.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    currentFront.y = sin(glm::radians(m_pitch));
    currentFront.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(currentFront);
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if (backEnd->getCamera()->m_firstMouse) {
        backEnd->getCamera()->m_lastX = xPos;
        backEnd->getCamera()->m_lastY = yPos;
        backEnd->getCamera()->m_firstMouse = false;
    }

    float xOffset = xPos - backEnd->getCamera()->m_lastX;
    float yOffset = backEnd->getCamera()->m_lastY - yPos;

    backEnd->getCamera()->m_lastX = xPos;
    backEnd->getCamera()->m_lastY = yPos;

    backEnd->getCamera()->processMouseMovement(xOffset, yOffset);
}

void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));
    backEnd->getCamera()->processMouseScroll(static_cast<float>(yOffset));
}