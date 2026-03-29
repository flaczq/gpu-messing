#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../managers/scene_manager.h"
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

    Camera* getCamera() const;
    Renderer* getRenderer() const;
    SceneManager* getSceneManager() const;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    unsigned int m_screenWidth{}, m_screenHeight{};

    GLFWwindow* m_window = nullptr;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<SceneManager> m_sceneManager;

    // textures
    //unsigned int diffuseMapTP, specularMapTP;

    // for showFps()
    double m_lastTime = glfwGetTime();
    unsigned int m_nbFrames = 0u;
    float m_lastFrame = 0.0f;

    void showFps(GLFWwindow* window, double currentTime);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);
};