#include "../../configs/gl_config.hpp"
#include "../../configs/log_config.hpp"
#include "../../core/back_end.h"
#include "../../graphics/material.h"
#include "../../graphics/shader.h"
#include "../../utils/component_utils.hpp"
#include "../../utils/component_utils.hpp"
#include "../../utils/enum_utils.hpp"
#include "../components/camera_component.hpp"
#include "../components/render_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "render_system.h"
#include <algorithm>

RenderSystem::RenderSystem() = default;

bool RenderSystem::init(GLFWwindow* window) {
    m_window = window;
    //m_light = {
    //    glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f)),
    //    glm::vec3(1.0f)
    //};

    // FIXME hardcoded max: 100
    m_opaqueQueue.reserve(100);
    m_stencilQueue.reserve(100);
    m_outlineQueue.reserve(100);
    m_blendingQueue.reserve(100);
    m_topLayerQueue.reserve(100);
    m_uiQueue.reserve(100);

    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));

    return true;
}

void RenderSystem::beginFrame(unsigned int width, unsigned int height) {
    glViewport(0, 0, width, height);

    // Z-depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderSystem::beginFrameMinimap(unsigned int minimapWidth, unsigned int minimapHeight) {
    unsigned int minimapX = minimapWidth * 3;
    unsigned int minimapY = minimapHeight * 3;
    // screen - minimap
    glViewport(minimapX, minimapY, minimapWidth, minimapHeight);

    glEnable(GL_SCISSOR_TEST);
    glScissor(minimapX, minimapY, minimapWidth, minimapHeight);

    glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderSystem::update(Registry& registry, float alpha) {
    RenderContext m_renderContext{};
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);

        // find primary camera for RenderContext
        if (camera->isPrimary) {
            m_renderContext.mainCamera = camera;
            m_renderContext.mainCameraTransform = transform;
            break;
        }
    }
    assert(m_renderContext.mainCamera != nullptr);
    assert(m_renderContext.mainCameraTransform != nullptr);

    for (Entity entity : registry.view<TransformComponent, RenderComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* render = registry.getComponent<RenderComponent>(entity);

        glm::mat4 modelMatrix = Utils::Component::calculateInterpolatedModelMatrix(*transform, alpha);
        glm::mat3 normalMatrix = Utils::Component::calculateNormalMatrix(modelMatrix);
        glm::vec3 interPosition = Utils::Component::calculateInterpolatedPosition(*transform, alpha);

        RenderQueueType queueType = render->queueType;
        RenderCommand command = {
            render->model.get(),
            render->material.get(),
            modelMatrix,
            normalMatrix,
            interPosition
        };
        _registerInQueue(queueType, command);
    }
}

void RenderSystem::_registerInQueue(RenderQueueType queueType, const RenderCommand& command) {
    switch (queueType) {
    case RenderQueueType::OPAQUE:
        m_opaqueQueue.push_back(command);
        break;
    case RenderQueueType::STENCIL:
        m_stencilQueue.push_back(command);
        break;
    case RenderQueueType::OUTLINE:
        m_outlineQueue.push_back(command);
        break;
    case RenderQueueType::BLENDING:
        m_blendingQueue.push_back(command);
        break;
    case RenderQueueType::TOP_LAYER:
        m_topLayerQueue.push_back(command);
        break;
    case RenderQueueType::UI:
        m_uiQueue.push_back(command);
        break;
    }
}

// ORDER: opaque -> transparent back-to-front
void RenderSystem::execute() {
    //    ┏┓┏┓┏┓┏┓┳┳┏┓  ┏┓┏┓┏┓┏┓
    //    ┃┃┃┃┣┫┃┃┃┃┣   ┃┃┣┫┗┓┗┓
    //    ┗┛┣┛┛┗┗┻┗┛┗┛  ┣┛┛┗┗┛┗┛
    //                          
    _sortQueueByMaterial(m_opaqueQueue);
    _renderSortedQueue(m_opaqueQueue, "opaque pass");

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
        _sortQueueByMaterial(m_stencilQueue);
        _renderSortedQueue(m_stencilQueue, "stencil pass");
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
        _sortQueueByMaterial(m_outlineQueue);
        _renderSortedQueue(m_outlineQueue, "outline pass");
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
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        // src: factor == source color vector, dst: factor == 1 - source color vector
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        // ---
        _sortQueueByDistance(m_blendingQueue);
        _renderSortedQueue(m_blendingQueue, "blending pass");
        // ---
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }

    //    ┏┳┓┏┓┏┓  ┓ ┏┓┓┏┏┓┳┓  ┏┓┏┓┏┓┏┓
    //     ┃ ┃┃┃┃  ┃ ┣┫┗┫┣ ┣┫  ┃┃┣┫┗┓┗┓
    //     ┻ ┗┛┣┛  ┗┛┛┗┗┛┗┛┛┗  ┣┛┛┗┗┛┗┛
    //                                 
    if (!m_topLayerQueue.empty()) {
        // always last
        glClear(GL_DEPTH_BUFFER_BIT);
        // different fov and planes for top layer
        m_camera->setFov(45.0f);
        m_camera->setNearPlane(0.01f);
        m_camera->setFarPlane(10.0f);
        m_camera->updateProjection(true);
        // ---
        _sortQueueByMaterial(m_topLayerQueue);
        _renderSortedQueue(m_topLayerQueue, "top layer pass");
        // ---
        m_camera->restoreDefaultProjection();
    }

    //    ┳┳•  ┏┓┏┓┏┓┏┓
    //    ┃┃┓  ┃┃┣┫┗┓┗┓
    //    ┗┛┗  ┣┛┛┗┗┛┗┛
    //                 
    if (!m_uiQueue.empty()) {
        _sortQueueByMaterial(m_uiQueue);
        _renderSortedQueue(m_uiQueue, "ui pass");
    }

    m_opaqueQueue.clear();
    m_stencilQueue.clear();
    m_outlineQueue.clear();
    m_blendingQueue.clear();
    m_topLayerQueue.clear();
    m_uiQueue.clear();
}

void RenderSystem::_sortQueueByMaterial(std::vector<RenderCommand>& queue) const {
    if (queue.empty()) {
        return;
    }

    // sort by material address
    std::sort(queue.begin(), queue.end(), [](const RenderCommand& cmd1, const RenderCommand& cmd2) {
        return cmd1.material < cmd2.material;
    });
}

void RenderSystem::_sortQueueByDistance(std::vector<RenderCommand>& queue) const {
    if (queue.empty()) {
        return;
    }

    // sort by the distance to the main camera (furthest to closest)
    glm::vec3 mainCameraPosition = m_renderContext.mainCameraTransform->position;
    std::sort(queue.begin(), queue.end(), [mainCameraPosition](const RenderCommand& cmd1, const RenderCommand& cmd2) {
        return glm::length(mainCameraPosition - cmd2.position) < glm::length(mainCameraPosition - cmd1.position);
    });
}

// TODO: use UBO
void RenderSystem::_renderSortedQueue(std::vector<RenderCommand>& queue, const std::string& name) const {
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
                currShader->setMat4fv("projection", m_renderContext.mainCamera->projection);
                currShader->setMat4fv("view", m_renderContext.mainCamera->view);
                currShader->setVec3fv("viewPos", cmd.position);
                currShader->setVec3fv("lightDir", m_light.direction);
                currShader->setVec3fv("lightColor", m_light.color);
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

void RenderSystem::renderImmediate() {
    std::vector<RenderImmediateCommand> queue;
    unsigned int VAO{};
    switch (m_renderDebugMode) {
    case RendererRenderDebugMode::NONE:
        return;
    case RendererRenderDebugMode::AABB:
        queue = m_physicsSystem.getAABBCommand();
        if (queue.empty()) {
            LOG_D("nothing to render immediately, sad QQ");
            return;
        }
        // one VAO to rule them all
        VAO = queue[0].VAO;
        break;
    }

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);

    auto shader = ResourceManager::getInstance().getShader("simple_shader");
    shader->use();
    shader->setMat4fv("projection", m_camera->getProjection());
    shader->setMat4fv("view", m_camera->getViewMatrix());
    for (auto& cmd : queue) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cmd.center);
        model = glm::scale(model, cmd.size);
        shader->setMat4fv("model", model);
        shader->setBool("hasMatColor", true);
        shader->setVec3fv("matColor", cmd.color);
        glDrawArrays(GL_LINES, 0, 24);
    }

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
}

// TUTORIAL: unused
void RenderSystem::_renderFrameBufferTexture() {
    unsigned int fbo{};
    glGenFramebuffers(1, &fbo);
    // off-screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int texture{};
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // target, attachment, textarget, texture, level
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // main window rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
}

void RenderSystem::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_STENCIL_TEST);
    //glDisable(GL_BLEND);

    glfwSwapBuffers(m_window);
}

void RenderSystem::endFrameMinimap() {
    glDisable(GL_SCISSOR_TEST);
}

void RenderSystem::toggleRenderMode() {
    m_renderMode = Utils::Enum::getNext(m_renderMode);
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_renderMode));
    LOG_D("Changed RenderMode to: " << Utils::Enum::getName(m_renderMode));
}

void RenderSystem::toggleRenderDebugMode() {
    m_renderDebugMode = Utils::Enum::getNext(m_renderDebugMode);
    LOG_D("Changed RenderDebugMode to: " << Utils::Enum::getName(m_renderDebugMode));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));
    backEnd->getCameraSystem().updateAspect(backEnd->getRegistry(), width, height);

    glViewport(0, 0, width, height);
}