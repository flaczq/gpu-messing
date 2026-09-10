#pragma once

#include "../../configs/math_config.hpp"
#include "../../utils/stats_constants.hpp"
#include "i_component.hpp"

struct CameraComponent : public IComponent {
    glm::mat4 view          { 1.0f };
    glm::mat4 projection    { 1.0f };
    float fov               = Constants::Stats::Camera::DEFAULT_FOV;
    float lastFov           = Constants::Stats::Camera::DEFAULT_FOV;
    float nearPlane         = Constants::Stats::Camera::NEAR_PLANE;
    float farPlane          = Constants::Stats::Camera::FAR_PLANE;
    bool isProjectionDirty  = false;
    bool isPrimary{};
    int screenWidth{};
    int screenHeight{};
    float aspect{};

    CameraComponent() = default;
    CameraComponent(int screenWidth_,
                    int screenHeight_,
                    bool isPrimary_ = false)
        : screenWidth(screenWidth_),
          screenHeight(screenHeight_),
          isPrimary(isPrimary_) {
        aspect = (float)screenWidth / (float)screenHeight;
    }
};