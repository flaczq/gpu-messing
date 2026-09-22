#pragma once

#include "../../configs/gl_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../ecs/registry.h"
#include "../../ecs/systems/ai_system.h"
#include "../../ecs/systems/camera_system.h"
#include "../../ecs/systems/dir_light_movement_system.h"
#include "../../ecs/systems/physics_system.h"
#include "../../ecs/systems/player_system.h"
#include "../../ecs/systems/render_system.h"
#include "../../ecs/systems/transform_system.h"
#include "../i_backend.h"

class OpenGLBackEnd : public IBackEnd {
public:
    OpenGLBackEnd();

    bool init(unsigned int width, unsigned int height) override;
    void run() override;
    virtual Registry& getRegistry() override { return m_registry; }
    virtual CameraSystem& getCameraSystem() override { return m_cameraSystem; }
    void setViewport(int x, int y, int width, int height) override;

private:
    GLFWwindow* m_window{};
    // ECS
    Registry m_registry{};
    AISystem m_aiSystem{};
    CameraSystem m_cameraSystem{};
    DirLightMovementSystem m_dirLightMovementSystem{};
    PhysicsSystem m_physicsSystem{};
    PlayerSystem m_playerSystem{};
    RenderSystem m_renderSystem; // OpenGLRenderer in constructor
    TransformSystem m_transformSystem{};

    // for showFps()
    double m_fpsLastTime{};
    unsigned int m_fpsNr = 0u;
    float m_fpsLastFrame = 0.0f;

    // time
    double m_lastTime = 0.0f;
    double m_accumulator = 0.0f;

    void _processGlobalInput();
    void _showFps(GLFWwindow* window, double currentTime);
};