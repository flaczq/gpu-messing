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
    void update(double dt);
    void lateUpdate();
    void toggleCameraMode();
    void toggleGodMode();

    glm::mat4 getViewMatrix() const { return m_view; }
    glm::vec3 getPosition() const { return m_position; }
    float getFov() const { return m_fov; }
    float getAspect() const { return m_aspect; }
    float getNearPlane() const { return NEAR_PLANE; }
    float getFarPlane() const { return FAR_PLANE; }

private:
    static constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr float MOVEMENT_SPEED = 5.0f;
    static constexpr float MOUSE_SENSITIVITY = 0.1f;
    static constexpr float NEAR_PLANE = 0.1f;
    static constexpr float FAR_PLANE = 100.0f;

    glm::mat4 m_view{};
    glm::vec3 m_position{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    float m_fov{};
    float m_aspect{};
    float m_yaw{};
    float m_pitch{};

    bool m_godMode = false;
    GLFWwindow* m_window = nullptr;
    CameraMode m_cameraMode = CameraMode::STANDING;

    void processKeyboard(CameraDirection direction, double dt);
    void processMouseScroll(float yoffset);
    void processMouseMovement(float xoffest, float yoffset, GLboolean constrainPitch = true);
    void updateCameraVectors();
    float getCameraModeHeight() const;
};