#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../ecs/registry.h"
#include "../ecs/systems/ai_system.h"
#include "../ecs/systems/camera_system.h"
#include "../ecs/systems/dir_light_movement_system.h"
#include "../ecs/systems/physics_system.h"
#include "../ecs/systems/player_system.h"
#include "../ecs/systems/render_system.h"
#include "../ecs/systems/transform_system.h"
#include "../managers/scene_manager.h"
#include <memory>

enum class GraphicsAPI {
    OPEN_GL,
    VULKAN
};

class BackEnd {
public:
    BackEnd(GraphicsAPI graphicsAPI, unsigned int width, unsigned int height);

    bool init();
    void run();
    Registry& getRegistry() { return m_registry; }
    CameraSystem& getCameraSystem() { return m_cameraSystem; }

private:
    unsigned int m_screenWidth{}, m_screenHeight{};
    unsigned int m_minimapWidth{}, m_minimapHeight{};

    Registry m_registry{};
    TransformSystem m_transformSystem{};
    PhysicsSystem m_physicsSystem{};
    PlayerSystem m_playerSystem{};
    AISystem m_aiSystem{};
    CameraSystem m_cameraSystem{};
    RenderSystem m_renderSystem{};
    DirLightMovementSystem m_dirLightMovementSystem{};

    GLFWwindow* m_window = nullptr;
    bool m_minimap = true;

    // textures
    //unsigned int diffuseMapTP, specularMapTP;

    // for showFps()
    double m_fpsLastTime{};
    unsigned int m_fpsNr = 0u;
    float m_fpsLastFrame = 0.0f;

    // time
    double m_lastTime = 0.0;
    double m_accumulator = 0.0;
    static constexpr double FIXED_DT = 1.0 / 60.0;

    void _processGlobalInput();
    void _showFps(GLFWwindow* window, double currentTime);
};