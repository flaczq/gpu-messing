#include "renderer.h"
#include "../configs/gl_config.hpp"
#include <string>
#include <iostream>

Renderer::Renderer(GLFWwindow* window) {
    m_window = window;
}

bool Renderer::init() {
    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));

    // set callbacks: window resize
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    return 1;
}

void Renderer::toggleRenderMode() {
    std::string renderModeStr;
    if (m_renderMode == RenderMode::STANDARD) {
        m_renderMode = RenderMode::WIREFRAME;
        renderModeStr = "WIREFRAME";
    } else if (m_renderMode == RenderMode::WIREFRAME) {
        m_renderMode = RenderMode::POINTCLOUD;
        renderModeStr = "POINTCLOUD";
    } else {
        m_renderMode = RenderMode::STANDARD;
        renderModeStr = "STANDARD";
    }
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));
    std::cout << "* Changed RenderMode to: " << renderModeStr << std::endl << std::endl;
}

void Renderer::beginFrame() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    glfwSwapBuffers(m_window);
}

void Renderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}