#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../configs/log_config.hpp"
#include "../cores/back_end.h"
#include "renderer.h"
#include <iostream>
#include <string>

Renderer& Renderer::getInstance() {
    static Renderer instance;
    return instance;
}

Renderer::Renderer() = default;

bool Renderer::init(GLFWwindow* window) {
    m_window = window;
    m_renderLight = {
        glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)),
        glm::vec3(1.0f)
    };

    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));

    return true;
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
    LOG_D("Changed RenderMode to: " << renderModeStr);
}

void Renderer::beginFrame() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Z-depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Renderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    glfwSwapBuffers(m_window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));
    backEnd->getCamera()->updateAspect(width, height);

    glViewport(0, 0, width, height);
}