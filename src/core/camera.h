#pragma once

#include "../utils/config.h"

class Camera {
public:
    Camera(GLFWwindow* window, unsigned int screenWidth, unsigned int screenHeight);

    bool init();
    void processInput(float dt);
    void processKeyboard(CameraDirection direction, float dt);
    void processMouseScroll(float yOffset);
    void processMouseMovement(float xOffest, float yOffset, GLboolean constrainPitch = true);

    glm::mat4 getViewMatrix() const;

    void toggleCameraMode();
    void toggleGodMode();

    glm::vec3 position;
    glm::vec3 front;
    float fov;
    float aspect;

private:
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    glm::quat orientation;
    bool firstMouse;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float movementSpeed;
    float mouseSensitivity;

    GLFWwindow* window = nullptr;
    CameraMode cameraMode = CameraMode::STANDING;
    bool godMode = false;

    void updateCameraVectors();
    float getCameraModeHeight() const;

    // must be static to be passed as a callback reference and so it needs to use camera->var
    static void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};