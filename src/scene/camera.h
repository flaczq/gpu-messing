#pragma once

#include "../utils/commongl.h"

enum class CameraDirection {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class CameraMode {
    STANDING,
    CROUCHING
};

class Camera {
public:
    Camera();
    // do not worry about deconstruction
    ~Camera() = default;

    bool init(GLFWwindow* window);
    void processInput(GLFWwindow* window, float 
    
    );
    void processKeyboard(CameraDirection direction, float deltaTime);
    void processMouseScroll(float yOffset);
    void processMouseMovement(float xOffest, float yOffset, GLboolean constrainPitch = true);

    glm::mat4 getViewMatrix() const;
    float getZoom() const;
    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;

    void changeCameraMode();

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
    CameraMode cameraMode = CameraMode::STANDING;

    void updateCameraVectors();
    float getCameraModeHeight() const;

    // must be static to be passed as a callback reference and so it needs to use camera->var
    static void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};