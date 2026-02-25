#pragma once

#include "commongl.h"
#include "shader.h"
#include "../libs/stb_image.h"

class Engine {
public:
    Engine(int w, int h);
    ~Engine();

    bool init();
    void run();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    int screen_w, screen_h;
    GLFWwindow* window;
    Shader* shader;
    GLuint VAO, VBO, EBO;
    GLuint texture1, texture2;
    std::vector<glm::vec3> cubePositions;

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    float uniformInterpolate = 0.4f;
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    void showFps(GLFWwindow* window);
    void processInput(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};