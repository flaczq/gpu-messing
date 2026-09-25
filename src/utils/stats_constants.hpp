#pragma once

#include "../configs/math_config.hpp"

namespace Constants {
    namespace Stats {
        namespace World {
            inline constexpr double FIXED_DT = 1.0f / 60.0f;
            inline constexpr glm::vec3 WORLD_UP { 0.0f, 1.0f, 0.0f };
        }

        namespace Camera {
            inline constexpr float MOUSE_SENSITIVITY    = 0.1f;
            inline constexpr float STANDING_OFFSET      = 1.75f;
            inline constexpr float CROUCHING_OFFSET     = 0.8f;
            inline constexpr float DEFAULT_FOV          = 60.0f;
            inline constexpr float NEAR_PLANE           = 0.1f;
            inline constexpr float FAR_PLANE            = 100.0f;
            inline constexpr float MIN_PITCH            = -75.0f;
            inline constexpr float MAX_PITCH            = 75.0f;
            inline constexpr float MIN_FOV              = 1.0f;
            inline constexpr float MAX_FOV              = 90.0f;
        }

        namespace Player {
            inline constexpr int MAX_HEALTH         = 100;
        }

        namespace Enemy_1 {
            inline constexpr int MAX_HEALTH = 66;
        }
        namespace Enemy_2 {
            inline constexpr int MAX_HEALTH = 67;
        }

        //m_minimapCamera->setViewPos(glm::vec3(8.0f, 20.0f, 15.0f));
        //m_minimapCamera->setPreViewPos(glm::vec3(8.0f, 20.0f, 15.0f));
        //m_minimapCamera->setYaw(-90.0f);
        //m_minimapCamera->setPitch(-70.0f);
    }
}