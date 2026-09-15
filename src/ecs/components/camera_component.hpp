#pragma once

#include "../../configs/math_config.hpp"
#include "../../utils/stats_constants.hpp"
#include "i_component.hpp"

struct CameraComponent : public IComponent {
    glm::mat4 view          { 1.0f };
    glm::mat4 projection    { 1.0f };
    float fov               = Constants::Stats::Camera::DEFAULT_FOV;
    float nearPlane         = Constants::Stats::Camera::NEAR_PLANE;
    float farPlane          = Constants::Stats::Camera::FAR_PLANE;
    float aspect            = 1.67f;
    // looking at (0,0,0)
    float yaw               = -69.0f;
    float pitch             = -1.0f;
    bool isPrimary{};

    //CameraComponent() = default;
    CameraComponent(bool isPrimary_ = true)
        : isPrimary(isPrimary_) {}
};