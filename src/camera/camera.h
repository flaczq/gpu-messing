#pragma once

#include "../utils/commongl.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    Camera();
    ~Camera();

    bool init(GLFWwindow* window);
    void processInput(GLFWwindow* window, float deltaTime);
    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseScroll(float yOffset);
    void processMouseMovement(float xOffest, float yOffset, GLboolean constrainPitch = true);

    glm::mat4 getViewMatrix();
    float getZoom();

private:
    glm::vec3 position;
    glm::vec3 front;
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
    float zoom;

    void updateCameraVectors();

    // must be static to be passed as a callback reference and so it needs to use camera->var
    static void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};