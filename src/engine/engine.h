#pragma once

#include "../utils/commongl.h"
#include "../utils/shader.h"
#include "../utils/texture.h"
#include "../camera/camera.h"

using UniformTypes = std::variant<float, std::string, bool>;

enum class RenderMode {
    STANDARD = GL_FILL,
    WIREFRAME = GL_LINE,
    POINTCLOUD = GL_POINT
};

class Engine {
public:
    Engine(int w, int h);
    ~Engine();

    bool init();
    void run();

    Camera& getCamera();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    int screen_w, screen_h;
    // shaders
    std::unique_ptr<Shader> objectShader;
    std::unique_ptr<Shader> lightShader;
    std::unique_ptr<Shader> gizmoShader;
    // textures
    unsigned int diffuseMap, specularMap;
    // camera
    Camera camera;

    GLFWwindow* window;
    GLuint VAO, lightVAO, gizmoVAO;
    GLuint VBO, gizmoVBO;
    std::vector<glm::vec3> cubePositions;
    std::vector<glm::vec3> pointLightPositions;

    // showFps()
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // uniform of changeables
    std::map<std::string, UniformTypes> uniformVars = {
        {"interpolate", 0.4f},
        {"spotlightOn", false},
        {"gizmoLength", 5.0f},
        {"gizmoNegative", false}
    };

    RenderMode renderMode = RenderMode::STANDARD;

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    void showFps(GLFWwindow* window);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);

    // must be static to be passed as a callback reference and so it needs to use engine->var
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};