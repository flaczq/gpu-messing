#include "renderer.h"

Renderer::Renderer(GLFWwindow* window) {
    this->window = window;
}

bool Renderer::init() {
    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(renderMode));

    // set callbacks: window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return 1;
}

void Renderer::toggleRenderMode() {
    std::string renderModeStr;
    if (renderMode == RenderMode::STANDARD) {
        renderMode = RenderMode::WIREFRAME;
        renderModeStr = "WIREFRAME";
    } else if (renderMode == RenderMode::WIREFRAME) {
        renderMode = RenderMode::POINTCLOUD;
        renderModeStr = "POINTCLOUD";
    } else {
        renderMode = RenderMode::STANDARD;
        renderModeStr = "STANDARD";
    }
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(renderMode));
    std::cout << "* Changed RenderMode to: " << renderModeStr << std::endl << std::endl;
}

void Renderer::beginFrame() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    glfwSwapBuffers(window);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}