#pragma once

#include "commongl.h"

class Camera {
public:
    Camera();
    ~Camera();

    bool init(GLFWwindow* window);
    void processInput(GLFWwindow* window);
    void processMouseScroll(float yOffset);
    void processMouseMovement(float xOffest, float yOffset, GLboolean constrainPitch = true);

    void updateDeltaTime(float deltaTime);
    glm::mat4 getViewMatrix();
    float getFov();
    float getZoom();

private:
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    glm::vec3 worldUp;
    float cameraSpeed;
    bool firstMouse;
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    float mouseSensitivity;
    float zoom;
    float fov;

    void updateCameraVectors();

    // must be static to be passed as a callback reference and so it needs to use camera->var
    static void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};