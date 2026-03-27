#include "renderer.h"

Renderer::Renderer() {
    window = nullptr;
}

bool Renderer::init(GLFWwindow* window) {
    this->window = window;

    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(renderMode));

    // set callbacks: window resize
    glfwSetFramebufferSizeCallback(this->window, framebuffer_size_callback);

    return 1;
}

RenderMode Renderer::getRenderMode() const {
    return renderMode;
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

void Renderer::beginFrame() const {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() const {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    glfwSwapBuffers(window);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}