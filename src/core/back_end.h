#pragma once

#include "../utils/config.h"
#include "../graphics/shader.h"
#include "../graphics/model.h"
#include "../graphics/mesh.h"
#include "../graphics/texture_primitive.h"
#include "../managers/scene_manager.h"
#include "camera.h"
#include "renderer.h"

class BackEnd {
public:
    BackEnd(GraphicsAPI graphicsAPI, unsigned int width, unsigned int height);
    ~BackEnd();

    bool init();
    void run();

    std::unique_ptr<Camera> camera;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<SceneManager> sceneManager;

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    unsigned int screenWidth, screenHeight;

    // textures
    //unsigned int diffuseMapTP, specularMapTP;

    // data known later then create in on the heap
    //std::unique_ptr<Model> modelSoldier;
    //std::unique_ptr<Mesh> mesh;

    // data known at start then create it with constructor on the stack
    //SceneManager sceneManager;

    GLFWwindow* window = nullptr;
    // shaders (little int ptr is created on stack but data itself on heap)
    //std::unique_ptr<Shader> objectShader = nullptr;
    //std::unique_ptr<Shader> lightShader = nullptr;

    // for showFps()
    double lastTime = glfwGetTime();
    unsigned int nbFrames = 0u;

    float lastFrame = 0.0f;

    void showFps(GLFWwindow* window, double currentTime);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);
};