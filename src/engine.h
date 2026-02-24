#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

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
    GLFWwindow* window;
    Shader* shader;
    GLuint VAO, VBO, EBO;
    GLuint texture1, texture2;
    int screen_w, screen_h;

    double lastTime = glfwGetTime();
    int nbFrames = 0;
    float uniformInterpolate = 0.8f;

    void showFps(GLFWwindow* window);
    void processInput(GLFWwindow* window);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};