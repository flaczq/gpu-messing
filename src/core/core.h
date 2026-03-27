#pragma once

#include "../utils/commongl.h"
#include "../renderer/shader.h"
#include "../renderer/model.h"
#include "../renderer/mesh.h"
#include "../renderer/texture_primitive.h"
#include "../renderer/renderer.h"
#include "../manager/scene_manager.h"
#include "camera.h"

using UniformTypes = std::variant<float, std::string, bool>;

class Core {
public:
    Core(unsigned int width, unsigned int height);
    ~Core();

    bool init();
    void run();

    Camera& getCamera();
    Renderer& getRenderer();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    unsigned int screenWidth, screenHeight;

    // shaders (little int ptr is created on stack but data itself on heap)
    std::unique_ptr<Shader> objectShader;
    std::unique_ptr<Shader> lightShader;
    std::unique_ptr<Shader> gridShader;
    std::unique_ptr<Shader> gizmoShader;
    std::unique_ptr<Shader> modelShader;

    // textures
    unsigned int diffuseMapTP, specularMapTP;

    // data known later then create in on the heap
    std::unique_ptr<Model> modelSoldier;
    //std::unique_ptr<Mesh> mesh;

    // data known at start then create it with constructor on the stack
    //Camera camera;
    Renderer renderer;
    SceneManager sceneManager;

    GLFWwindow* window;
    GLuint VAO, lightVAO, gizmoVAO, gridVAO;
    GLuint VBO, gizmoVBO, gridVBO;
    std::vector<glm::vec3> cubePositions;
    std::vector<glm::vec3> pointLightPositions;
    std::vector<glm::vec3> gridPositions;

    // for showFps()
    double lastTime = glfwGetTime();
    unsigned int nbFrames = 0u;

    float lastFrame = 0.0f;

    // uniform of changeables
    std::map<std::string, UniformTypes> uniformVars = {
        {"interpolate", 0.4f},
        {"spotlightOn", false},
        {"gizmoLength", 10.0f},
        {"gizmoNegative", false}
    };


    void showFps(GLFWwindow* window, double currentTime);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);
};