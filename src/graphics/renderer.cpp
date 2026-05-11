#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../cores/back_end.h"
#include "../game/camera.h"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "renderer.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

Renderer& Renderer::getInstance() {
    static Renderer instance;
    return instance;
}

Renderer::Renderer() = default;

bool Renderer::init(GLFWwindow* window, Camera* camera) {
    m_window = window;
    m_camera = camera;
    m_rendererLight = {
        glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)),
        glm::vec3(1.0f)
    };

    // FIXME hardcoded max: 100
    m_opaqueQueue.reserve(100);
    m_stencilQueue.reserve(100);
    m_outlineQueue.reserve(100);
    m_blendingQueue.reserve(100);

    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));

    return true;
}

void Renderer::toggleRenderMode() {
    std::string renderModeStr;
    if (m_renderMode == RendererRenderMode::STANDARD) {
        m_renderMode = RendererRenderMode::WIREFRAME;
        renderModeStr = "WIREFRAME";
    } else if (m_renderMode == RendererRenderMode::WIREFRAME) {
        m_renderMode = RendererRenderMode::POINTCLOUD;
        renderModeStr = "POINTCLOUD";
    } else {
        m_renderMode = RendererRenderMode::STANDARD;
        renderModeStr = "STANDARD";
    }
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));
    LOG_D("Changed RenderMode to: " << renderModeStr);
}

void Renderer::beginFrame() {
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    glViewport(0, 0, width, height);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Z-depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Stencil test
    /*if (m_stencilReqd) {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0x00);
    }*/

    // Blending
    /*if (m_blendingReqd) {
        glEnable(GL_BLEND);
        // src: factor == source color vector, dst: factor == 1 - source color vector
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    }*/
}

void Renderer::registerInQueue(RendererQueueType queueType, const RendererCommand& command) {
    switch (queueType) {
    case RendererQueueType::OPAQUE:
        m_opaqueQueue.push_back(command);
        break;
    case RendererQueueType::STENCIL:
        m_stencilQueue.push_back(command);
        break;
    case RendererQueueType::OUTLINE:
        m_outlineQueue.push_back(command);
        break;
    case RendererQueueType::BLENDING:
        m_blendingQueue.push_back(command);
        break;
    }
}

// execute drawing commands from queues
// ORDER: opaque -> transparent back-to-front
void Renderer::flush() {
    //    ┏┓┏┓┏┓┏┓┳┳┏┓  ┏┓┏┓┏┓┏┓
    //    ┃┃┃┃┣┫┃┃┃┃┣   ┃┃┣┫┗┓┗┓
    //    ┗┛┣┛┛┗┗┻┗┛┗┛  ┣┛┛┗┗┛┗┛
    //                          
    sortQueueByMaterial(m_opaqueQueue);
    renderSortedQueue(m_opaqueQueue, "opaque pass");

    //    ┏┓┏┳┓┏┓┳┓┏┓•┓   ┏┓┏┓┏┓┏┓
    //    ┗┓ ┃ ┣ ┃┃┃ ┓┃   ┃┃┣┫┗┓┗┓
    //    ┗┛ ┻ ┗┛┛┗┗┛┗┗┛  ┣┛┛┗┗┛┗┛
    //                            
    if (!m_stencilQueue.empty()) {
        glEnable(GL_STENCIL_TEST);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        // ---
        sortQueueByMaterial(m_stencilQueue);
        renderSortedQueue(m_stencilQueue, "stencil pass");
    }

    //    ┏┓┳┳┏┳┓┓ ┳┳┓┏┓  ┏┓┏┓┏┓┏┓
    //    ┃┃┃┃ ┃ ┃ ┃┃┃┣   ┃┃┣┫┗┓┗┓
    //    ┗┛┗┛ ┻ ┗┛┻┛┗┗┛  ┣┛┛┗┗┛┗┛
    //                            
    if (!m_outlineQueue.empty()) {
        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
        // ---
        sortQueueByMaterial(m_outlineQueue);
        renderSortedQueue(m_outlineQueue, "outline pass");
        // ---
        glEnable(GL_DEPTH_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
        glDisable(GL_STENCIL_TEST);
    }

    //    ┳┓┓ ┏┓┳┓┳┓•┳┓┏┓  ┏┓┏┓┏┓┏┓
    //    ┣┫┃ ┣ ┃┃┃┃┓┃┃┃┓  ┃┃┣┫┗┓┗┓
    //    ┻┛┗┛┗┛┛┗┻┛┗┛┗┗┛  ┣┛┛┗┗┛┗┛
    //                             
    if (!m_blendingQueue.empty()) {
        glEnable(GL_BLEND);
        // src: factor == source color vector, dst: factor == 1 - source color vector
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        // ---
        sortQueueByDistance(m_blendingQueue);
        renderSortedQueue(m_blendingQueue, "blending pass");
        // ---
        glDisable(GL_BLEND);
    }

    m_opaqueQueue.clear();
    m_stencilQueue.clear();
    m_outlineQueue.clear();
    m_blendingQueue.clear();
}

void Renderer::sortQueueByMaterial(std::vector<RendererCommand>& queue) const {
    if (queue.empty()) {
        return;
    }

    // sort by material address
    std::sort(queue.begin(), queue.end(), [](const RendererCommand& cmd1, const RendererCommand& cmd2) {
        return cmd1.material < cmd2.material;
    });
}

void Renderer::sortQueueByDistance(std::vector<RendererCommand>& queue) const {
    if (queue.empty()) {
        return;
    }

    /*std::map<float, RendererCommand> distanceSorted;
    for (size_t i{}; i < m_blendingQueue.size(); i++) {
        float distance = glm::length(m_camera->getViewPos() - m_blendingQueue[i].position);
        distanceSorted[distance] = m_blendingQueue[i];
    }*/

    // sort by distance to camera (furthest to closest)
    auto cameraPos = m_camera->getViewPos();
    std::sort(queue.begin(), queue.end(), [cameraPos](const RendererCommand& cmd1, const RendererCommand& cmd2) {
        return glm::length(cameraPos - cmd2.position) < glm::length(cameraPos - cmd1.position);
    });
}

// TODO: use UBO
void Renderer::renderSortedQueue(std::vector<RendererCommand>& queue, const std::string& name) const {
    if (queue.empty()) {
        //LOG_D("Empty queue for: " << name << " - nothing to render");
        return;
    } else {
        //LOG_D("--- " << name);
    }

    // shader sorting
    Material* lastMaterial = nullptr;
    Shader* lastShader = nullptr;
    for (auto& cmd : queue) {
        Material* currMaterial = cmd.material;
        if (currMaterial != lastMaterial) {
            //const std::string& lastMN = (lastMaterial != nullptr) ? lastMaterial->getName() : "NULL";
            //LOG_D("Switching material from: " << lastMN << " to: " << currMaterial->getName());
            Shader* currShader = currMaterial->getShader();
            if (currShader != lastShader) {
                //unsigned int lastSI = (lastShader != nullptr) ? lastShader->getID() : 999;
                //LOG_D("Switching shader from: " << lastSI << " to: " << currShader->getID());
                currShader->use();

                // draws per-shader (rarely)
                currShader->setMat4fv("projection", m_camera->getProjection());
                currShader->setMat4fv("view", m_camera->getViewMatrix());
                currShader->setVec3fv("viewPos", m_camera->getViewPos());
                currShader->setVec3fv("lightDir", m_rendererLight.direction);
                currShader->setVec3fv("lightColor", m_rendererLight.color);
                //LOG_D("per-shader draws with shader: " << currShader->getID());

                lastShader = currShader;
            }

            // draws per-material (sometimes)
            currMaterial->apply();
            //LOG_D("per-material draws with shader: " << currShader->getID());

            lastMaterial = currMaterial;
        }

        assert(lastMaterial != nullptr);
        assert(lastShader != nullptr);

        // draws per-entity (always)
        lastShader->setMat4fv("model", cmd.modelMatrix);
        lastShader->setMat3fv("normalMatrix", cmd.normalMatrix);
        //LOG_D("per-entity draws with shader: " << lastShader->getID());

        cmd.model->draw(*lastShader);
    }
}

void Renderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_STENCIL_TEST);
    //glDisable(GL_BLEND);

    glfwSwapBuffers(m_window);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));
    backEnd->getCamera()->updateAspect(width, height);

    glViewport(0, 0, width, height);
}