#include "camera.h"
#include "engine.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 0.0f, 3.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRight = glm::vec3(0.0f, 1.0f, 0.0f); //fake
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    firstMouse = true;
    yaw = -90.0f;
    pitch = 0.0f;
    lastX = 800.0f / 2.0f;
    lastY = 600.0f / 2.0f;
    movementSpeed = 10.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;
    fov = 45.0f;

    updateCameraVectors();
};
Camera::~Camera() {
};

bool Camera::init(GLFWwindow* window) {
    // set callbacks
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return 1;
}

void Camera::processInput(GLFWwindow* window, float deltaTime) {
    // EXIT
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        processKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        processKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        processKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        processKeyboard(RIGHT, deltaTime);
    }
};

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD) {
        position += cameraFront * velocity;
    }
    if (direction == BACKWARD) {
        position -= cameraFront * velocity;
    }
    if (direction == LEFT) {
        position -= cameraRight * velocity;
    }
    if (direction == RIGHT) {
        position += cameraRight * velocity;
    }

    // FPS stay on the ground, boy
    position.y = 0.0f;
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
        if (pitch > 45.0f) {
            pitch = 45.0f;
        }
        if (pitch < -45.0f) {
            pitch = -45.0f;
        }
    }

    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + cameraFront, cameraUp);
    // TODO custom lookAt
    //return glm::mat4(1.0f);
};

float Camera::getFov() {
    return fov;
};

float Camera::getZoom() {
    return zoom;
};

void Camera::updateCameraVectors() {
    glm::vec3 front = glm::vec3(0.0f, 0.0f, 0.0f);
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    float xPos = static_cast<float>(xPosIn);
    float yPos = static_cast<float>(yPosIn);
    Camera& camera = engine->getCamera();

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
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
    Camera& camera = engine->getCamera();
    camera.processMouseScroll(static_cast<float>(yOffset));
}