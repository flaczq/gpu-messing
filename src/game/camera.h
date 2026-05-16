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
    Camera(unsigned int width, unsigned int height);

    bool init();
    void saveState();
    void processInput();
    void fixedUpdate(float fixedt);
    void updateVectors();
    void updateView(float alpha);
    void updateProjection(bool force = false);
    void restoreDefaultProjection();
    void updateAspect(int width, int height);
    void toggleCameraMode();
    void toggleGodMode();

    glm::mat4 getViewMatrix() const { return m_view; }
    glm::vec3 getViewPos() const { return m_viewPos; }
    void setViewPos(glm::vec3 viewPos) { m_viewPos = viewPos; }
    glm::vec3 getPreViewPos() const { return m_preViewPos; }
    void setPreViewPos(glm::vec3 preViewPos) { m_preViewPos = preViewPos; }
    glm::mat4 getProjection() const { return m_projection; }
    void setProjection(glm::mat4 projection) { m_projection = projection; }
    float getYaw() const { return m_yaw; }
    void setYaw(float yaw) { m_yaw = yaw; }
    float getPitch() const { return m_pitch; }
    void setPitch(float pitch) { m_pitch = pitch; }
    float getFov() const { return m_fov; }
    void setFov(float fov) { m_fov = fov; m_projectionDirty = true; }
    float getAspect() const { return m_aspect; }
    float getNearPlane() const { return m_nearPlane; }
    void setNearPlane(float near_plane) { m_nearPlane = near_plane; m_projectionDirty = true; }
    float getFarPlane() const { return m_farPlane; }
    void setFarPlane(float far_plane) { m_farPlane = far_plane; m_projectionDirty = true; }

private:
    static constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr float MOVEMENT_SPEED = 5.0f;
    static constexpr float MOUSE_SENSITIVITY = 0.1f;
    static constexpr float FOV = 60.0f;
    static constexpr float NEAR_PLANE = 0.1f;
    static constexpr float FAR_PLANE = 100.0f;
    static constexpr float MIN_PITCH = -75.0f;
    static constexpr float MAX_PITCH = 75.0f;
    static constexpr float MIN_FOV = 1.0f;
    static constexpr float MAX_FOV = 90.0f;

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
    float m_nearPlane{};
    float m_farPlane{};
    float m_aspect{};
    bool m_currDirections[6] = { false };

    bool m_projectionDirty = false;
    bool m_godModeChanged = false;
    bool m_cameraModeChanged = false;
    bool m_godMode = false;
    CameraMode m_cameraMode = CameraMode::STANDING;

    void processMouseScroll(float yoffset);
    void processMouseMovement(float xoffest, float yoffset, GLboolean constrainPitch = true);
    float getCameraModeHeight() const;
};