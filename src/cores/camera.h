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
    void update();
    void lateUpdate(double dt);
    glm::mat4 getViewMatrix() const;
    void toggleCameraMode();
    void toggleGodMode();

    glm::vec3 getPosition() const { return m_position; }
    float getFov() const { return m_fov; }
    float getAspect() const { return m_aspect; }

private:
    glm::vec3 m_position{};
    glm::vec3 m_previousPosition{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    glm::vec3 m_worldUp{};
    glm::quat m_orientation{};
    float m_fov{};
    float m_aspect{};
    float m_yaw{};
    float m_previousYaw{};
    float m_pitch{};
    float m_previousPitch{};
    float m_movementSpeed{};
    float m_mouseSensitivity{};

    bool m_godMode = false;
    bool m_cKeyPressed = false;
    GLFWwindow* m_window = nullptr;
    CameraMode m_cameraMode = CameraMode::STANDING;

    void processKeyboard(CameraDirection direction, double dt);
    void processMouseScroll(float yoffset);
    void processMouseMovement(float xoffest, float yoffset, GLboolean constrainPitch = true);
    void updateCameraVectors();
    float getCameraModeHeight() const;
};