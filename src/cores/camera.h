#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"

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
    Camera(GLFWwindow* window, unsigned int screenWidth, unsigned int screenHeight);

    bool init();
    void processInput(float dt);
    void processKeyboard(CameraDirection direction, float dt);
    void processMouseScroll(float yOffset);
    void processMouseMovement(float xOffest, float yOffset, GLboolean constrainPitch = true);

    glm::mat4 getViewMatrix() const;
    glm::vec3 getPosition() const;
    float getFov() const;
    float getAspect() const;

    void toggleCameraMode();
    void toggleGodMode();


private:
    glm::vec3 m_position{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};
    glm::quat m_orientation{};
    bool m_firstMouse{};
    float m_fov{};
    float m_aspect{};
    float m_yaw{};
    float m_pitch{};
    float m_lastX{};
    float m_lastY{};
    float m_movementSpeed{};
    float m_mouseSensitivity{};

    GLFWwindow* m_window = nullptr;
    CameraMode m_cameraMode = CameraMode::STANDING;
    bool m_godMode = false;

    void updateCameraVectors();
    float getCameraModeHeight() const;

    // must be static to be passed as a callback reference and so it needs to use camera->var
    static void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
    static void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
};