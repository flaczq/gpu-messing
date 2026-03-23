#include "camera.h"
#include "../core/core.h"

Camera::Camera() {
    position = glm::vec3(6.0f, 1.0f, 6.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::vec3(0.0f, 1.0f, 0.0f); //fake
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); //fake

    // CameraMode::STANDING
    position.y = getCameraModeHeight();

    // looking at (0,0,0)
    yaw = -135.0f;  
    pitch = -11.5f;

    firstMouse = true;
    lastX = 800.0f / 2.0f;
    lastY = 600.0f / 2.0f;
    movementSpeed = 10.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;

    updateCameraVectors();
}

bool Camera::init(GLFWwindow* window) {
    // set callbacks: mouse, scroll
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return 1;
}

// continuous key clicks -> movement
void Camera::processInput(GLFWwindow* window, float deltaTime) {
    // EXIT
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // MOVEMENT
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        processKeyboard(CameraDirection::FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        processKeyboard(CameraDirection::BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        processKeyboard(CameraDirection::LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        processKeyboard(CameraDirection::RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        processKeyboard(CameraDirection::UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        processKeyboard(CameraDirection::DOWN, deltaTime);
    }
};

void Camera::processKeyboard(CameraDirection direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == CameraDirection::FORWARD) {
        position += front * velocity;
    }
    if (direction == CameraDirection::BACKWARD) {
        position -= front * velocity;
    }
    if (direction == CameraDirection::LEFT) {
        position -= right * velocity;
    }
    if (direction == CameraDirection::RIGHT) {
        position += right * velocity;
    }
    // GOD MODE ACTIVATED
    if (godMode) {
        if (direction == CameraDirection::UP) {
            position += up * velocity;
        }
        if (direction == CameraDirection::DOWN) {
            position -= up * velocity;
        }
    } else {
        // FPS stay on the ground, peasant
        position.y = getCameraModeHeight();
    }
}

void Camera::processMouseScroll(float yOffset) {
    zoom -= (float)yOffset;
    if (zoom < 1.0f) {
        zoom = 1.0f;
    }
    if (zoom > 90.0f) {
        zoom = 90.0f;
    }
};

void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch) {
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (constrainPitch) {
        if (pitch < -90.0f) {
            pitch = -90.0f;
        }
        if (pitch > 90.0f) {
            pitch = 90.0f;
        }
    }

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    // camera position, where you looking at, up vector
    return glm::lookAt(position, position + front, up);
};

float Camera::getZoom() const {
    return zoom;
};

glm::vec3 Camera::getPosition() const {
    return position;
};

glm::vec3 Camera::getFront() const {
    return front;
};

float Camera::getCameraModeHeight() const {
    if (cameraMode == CameraMode::STANDING) {
        return 1.75f;
    }
    return 0.85f;

}

void Camera::changeCameraMode() {
    std::string cameraModeStr;
    if (cameraMode == CameraMode::STANDING) {
        cameraMode = CameraMode::CROUCHING;
        cameraModeStr = "CROUCHING";
    } else {
        cameraMode = CameraMode::STANDING;
        cameraModeStr = "STANDING";
    }
    position.y = getCameraModeHeight();
    std::cout << "* Changed camera mode to: " << cameraModeStr << std::endl << std::endl;
};

void Camera::changeGodMode() {
    godMode = !godMode;
    std::cout << "* Changed god mode to: " << godMode << std::endl << std::endl;
}

void Camera::updateCameraVectors() {
    glm::vec3 currFront = glm::vec3(0.0f, 0.0f, 0.0f);
    currFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    currFront.y = sin(glm::radians(pitch));
    currFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(currFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
    Core* core = static_cast<Core*>(glfwGetWindowUserPointer(window));
    Camera& camera = core->getCamera();
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);

    if (camera.firstMouse) {
        camera.lastX = xPos;
        camera.lastY = yPos;
        camera.firstMouse = false;
    }

    float xOffset = xPos - camera.lastX;
    float yOffset = camera.lastY - yPos;

    camera.lastX = xPos;
    camera.lastY = yPos;

    camera.processMouseMovement(xOffset, yOffset);
}

void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
    Core* core = static_cast<Core*>(glfwGetWindowUserPointer(window));
    Camera& camera = core->getCamera();

    camera.processMouseScroll(static_cast<float>(yOffset));
}