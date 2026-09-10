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

class Camera {
public:
    Camera(unsigned int width, unsigned int height);

    bool init();
    void processInput();
    void follow(TransformComponent* transform);
    void updateView(float alpha);
    void updateProjection(bool force = false);
    void restoreDefaultProjection();
    void updateAspect(int width, int height);

    TransformComponent* getFollowedTransform() const { return m_followedTransform; }
    glm::mat4 getViewMatrix() const { return m_view; }
    glm::mat4 getProjection() const { return m_projection; }
    void setProjection(glm::mat4 projection) { m_projection = projection; }
    float getFov() const { return m_fov; }
    void setFov(float fov) { m_fov = fov; m_projectionDirty = true; }
    float getAspect() const { return m_aspect; }
    float getNearPlane() const { return m_nearPlane; }
    void setNearPlane(float near_plane) { m_nearPlane = near_plane; m_projectionDirty = true; }
    float getFarPlane() const { return m_farPlane; }
    void setFarPlane(float far_plane) { m_farPlane = far_plane; m_projectionDirty = true; }

private:
    static constexpr float MOUSE_SENSITIVITY = 0.1f;
    static constexpr float STANDING_OFFSET = 1.75f;
    static constexpr float CROUCHING_OFFSET = 0.8f;
    static constexpr float FOV = 60.0f;
    static constexpr float NEAR_PLANE = 0.1f;
    static constexpr float FAR_PLANE = 100.0f;
    static constexpr float MIN_PITCH = -75.0f;
    static constexpr float MAX_PITCH = 75.0f;
    static constexpr float MIN_FOV = 1.0f;
    static constexpr float MAX_FOV = 90.0f;

    TransformComponent* m_followedTransform = nullptr;
    glm::mat4 m_view{};
    glm::mat4 m_projection{};
    float m_fov{}, m_lastFov{};
    float m_nearPlane{};
    float m_farPlane{};
    float m_aspect{};
    bool m_projectionDirty = false;

    void processMouseScroll(float yoffset);
    void processMouseMovement(float xoffest, float yoffset, GLboolean clampPitch = true);
};