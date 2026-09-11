#pragma once

#include "../../configs/math_config.hpp"
#include "../../utils/stats_constants.hpp"
#include "i_component.hpp"

enum class CameraType {
    PRIMARY,
    MINIMAP
};

struct CameraComponent : public IComponent {
    glm::mat4 view          { 1.0f };
    glm::mat4 projection    { 1.0f };
    float fov               = Constants::Stats::Camera::DEFAULT_FOV;
    float nearPlane         = Constants::Stats::Camera::NEAR_PLANE;
    float farPlane          = Constants::Stats::Camera::FAR_PLANE;
    CameraType type{};
    int screenWidth{};
    int screenHeight{};
    float aspect{};

    CameraComponent() = default;
    CameraComponent(int screenWidth_,
                    int screenHeight_,
                    CameraType type_ = CameraType::PRIMARY)
        : screenWidth(screenWidth_),
          screenHeight(screenHeight_),
          type(type_) {
        aspect = (float)screenWidth / (float)screenHeight;
    }
};