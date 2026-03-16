#pragma once

#include "../utils/commongl.h"
#include "../renderer/shader.h"
#include "../renderer/textures.h"
#include "../renderer/mesh.h"
#include "../renderer/model.h"
#include "../renderer/renderer.h"
#include "../scene/camera.h"

using UniformTypes = std::variant<float, std::string, bool>;

class Core {
public:
    Core(int w, int h);
    ~Core();

    bool init();
    void run();

    Camera& getCamera();
    Renderer& getRenderer();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    int screen_w, screen_h;

    // shaders
    std::unique_ptr<Shader> objectShader;
    std::unique_ptr<Shader> lightShader;
    std::unique_ptr<Shader> gizmoShader;
    std::unique_ptr<Shader> gridShader;

    // textures
    unsigned int diffuseMap, specularMap;

    // mesh, model, camera, renderer
    Mesh mesh;
    Model model;
    Camera camera;
    Renderer renderer;

    GLFWwindow* window;
    GLuint VAO, lightVAO, gizmoVAO, gridVAO;
    GLuint VBO, gizmoVBO, gridVBO;
    std::vector<glm::vec3> cubePositions;
    std::vector<glm::vec3> pointLightPositions;
    std::vector<glm::vec3> gridPositions;

    // showFps()
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // uniform of changeables
    std::map<std::string, UniformTypes> uniformVars = {
        {"interpolate", 0.4f},
        {"spotlightOn", false},
        {"gizmoLength", 10.0f},
        {"gizmoNegative", false}
    };

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    void showFps(GLFWwindow* window);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);

    // must be static to be passed as a callback reference and so it needs to use core->var
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};