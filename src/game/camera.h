#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include <vector>

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
    Camera(unsigned int screenWidth, unsigned int screenHeight);

    bool init();
    void saveState();
    void processInput();
    void fixedUpdate(float fixedt);
    void update(float alpha);
    void updateAspect(int width, int height);
    void toggleCameraMode();
    void toggleGodMode();

    glm::mat4 getViewMatrix() const { return m_view; }
    glm::vec3 getViewPos() const { return m_viewPos; }
    glm::mat4 getProjection() const { return m_projection; }
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
    glm::vec3 m_viewPos{};
    glm::vec3 m_preViewPos{};
    glm::mat4 m_projection{};
    glm::vec3 m_front{};
    glm::vec3 m_up{};
    glm::vec3 m_right{};
    float m_yaw{};
    float m_pitch{};
    float m_fov{};
    float m_aspect{};
    bool m_currDirections[6] = { false };

    bool m_projectionDirty = false;
    bool m_godModeChanged = false;
    bool m_cameraModeChanged = false;
    bool m_godMode = false;
    CameraMode m_cameraMode = CameraMode::STANDING;

    void processMouseScroll(float yoffset);
    void processMouseMovement(float xoffest, float yoffset, GLboolean constrainPitch = true);
    void updateCameraVectors();
    void updateProjection();
    float getCameraModeHeight() const;
};