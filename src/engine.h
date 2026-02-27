#pragma once

#include "commongl.h"
#include "shader.h"
#include "camera.h"
#include "../libs/stb_image.h"

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
    Shader* shader;
    Camera camera;

    GLFWwindow* window;
    GLuint VAO, VBO, EBO;
    GLuint texture1, texture2;
    std::vector<glm::vec3> cubePositions;

    // showFps()
    double lastTime = glfwGetTime();
    int nbFrames = 0;

    // changeable variables
    float uniformInterpolate = 0.4f;

    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    void showFps(GLFWwindow* window);
    void displayPosition(glm::mat4 viewMatrix);
    void displayCameraAngles(glm::mat4 viewMatrix);

    // must be static to be passed as a callback reference and so it needs to use engine->var
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};