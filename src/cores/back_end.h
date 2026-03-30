#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../managers/scene_manager.h"
#include "../physics/physics_world.h"
#include "camera.h"
#include "renderer.h"
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
    void processCommonInput();

    Camera* getCamera() const;
    Renderer* getRenderer() const;
    SceneManager* getSceneManager() const;
    PhysicsWorld* getPhysicsWorld() const;

private:
    unsigned int m_screenWidth{}, m_screenHeight{};

    GLFWwindow* m_window = nullptr;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<SceneManager> m_sceneManager;
    std::unique_ptr<PhysicsWorld> m_physicsWorld;

    // textures
    //unsigned int diffuseMapTP, specularMapTP;

    // for showFps()
    double m_fpsLastTime{};
    unsigned int m_fpsNr = 0u;
    float m_fpsLastFrame = 0.0f;

    // physics
    double m_lastTime = 0.0;
    double m_accumulator = 0.0;
    static constexpr double FIXED_DT = 1.0 / 60.0;

    void showFps(GLFWwindow* window, double currentTime);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);
};