#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../cores/back_end.h"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
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
    m_rendererLight = {
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Z-depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // Stencil test
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void Renderer::drawStandard(const RendererCommand& command) {
    // TODO shader sorting
    glStencilMask(0x00);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    draw(command);
}

void Renderer::drawWithStencilWrite(const RendererCommand& command) {
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    draw(command);
}

void Renderer::drawStencilOutline(const RendererCommand& command) {
    glStencilMask(0x00);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glDisable(GL_DEPTH_TEST);
    draw(command);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

void Renderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    glfwSwapBuffers(m_window);
}

void Renderer::draw(const RendererCommand& command) const {
    // transformation matrix: clip = projectionM * viewM * modelM * local
    // 1. local * modelM            -> world
    // 2. world * viewM             -> space (lookAt())
    // 3. space * projectionM       -> clip
    // 4. clip  * viewportTransform -> screen
    command.material->apply();

    auto* shader = command.material->getShader();
    shader->setMat4fv("projection", command.projection);
    shader->setMat4fv("view", command.view);
    shader->setVec3fv("viewPos", command.viewPos);
    shader->setMat4fv("model", command.modelMatrix);
    shader->setMat3fv("normalMatrix", command.normalMatrix);
    shader->setVec3fv("lightDir", m_rendererLight.direction);
    shader->setVec3fv("lightColor", m_rendererLight.color);

    command.model->draw(*shader);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));
    backEnd->getCamera()->updateAspect(width, height);

    glViewport(0, 0, width, height);
}