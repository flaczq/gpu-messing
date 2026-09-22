#include "../../api/i_backend.h"
#include "../../api/i_renderer.h"
#include "../../configs/log_config.hpp"
#include "../../graphics/material.h"
#include "../../graphics/shader.h"
#include "../../managers/resource_manager.h"
#include "../../utils/color_constants.hpp"
#include "../../utils/enum_utils.hpp"
#include "../../utils/math_utils.hpp"
#include "../components/camera_component.hpp"
#include "../components/dir_light_movement_component.hpp"
#include "../components/physics_component.hpp"
#include "../components/render_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "../systems/camera_system.h"
#include "render_system.h"
#include <algorithm>
#include <memory>

RenderSystem::RenderSystem(std::unique_ptr<IRenderer> renderer)
    : m_renderer(std::move(renderer))
{
}

RenderSystem::~RenderSystem() = default;

bool RenderSystem::init() {
    // FIXME hardcoded max: 100
    m_opaqueQueue.reserve(100);
    m_stencilQueue.reserve(100);
    m_outlineQueue.reserve(100);
    m_blendingQueue.reserve(100);
    m_topLayerQueue.reserve(100);
    m_uiQueue.reserve(100);

    m_renderer->init();

    return true;
}

void RenderSystem::beginFrame(int width, int height) const {
    RendererState state{
        .depthTest = true,
        .faceCulling = true,
        .clearColor = glm::vec4(0.2f, 0.1f, 0.1f, 1.0f),
        .clearColorBB = true,
        .clearDepthBB = true,
        .clearStencilBB = true
    };
    m_renderer->beginFrame(0, 0, width, height, state);
}

void RenderSystem::beginFrameMinimap(int minimapWidth, int minimapHeight) {
    RendererState state{
        .scissorTest = true,
        .clearColor = glm::vec4(0.2f, 0.1f, 0.1f, 1.0f),
        .clearColorBB = true,
        .clearDepthBB = true
    };
    m_renderer->beginFrame(minimapWidth * 3, minimapHeight * 3, minimapWidth, minimapHeight, state);
}

void RenderSystem::update(Registry& registry, float alpha) {
    m_renderContext = RenderContext{};
    // CAMERA+PLAYER
    for (Entity entity : registry.view<TransformComponent, CameraComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* camera = registry.getComponent<CameraComponent>(entity);
        //auto* optionalPlayer = registry.getComponent<PlayerComponent>(entity);

        // find primary camera for RenderContext
        if (camera->isPrimary) {
            float yOffset = 0.0f;
            // FIXME make it more abstract because Camera can follow non-Player entity
            //if (optionalPlayer != nullptr) {
            //    yOffset = optionalPlayer->isCrouching ? Constants::Stats::Camera::CROUCHING_OFFSET : Constants::Stats::Camera::STANDING_OFFSET;
            //}
            // view and projection for most passes
            m_renderContext.cameraView = Utils::Math::calculateView(transform->position,
                                                                    transform->prevPosition,
                                                                    camera->yaw,
                                                                    camera->pitch,
                                                                    alpha,
                                                                    yOffset);
            m_renderContext.cameraProjection = Utils::Math::calculateProjection(
                camera->fov,
                camera->aspect,
                camera->nearPlane,
                camera->farPlane
            );
            m_renderContext.cameraAspect = camera->aspect;
            m_renderContext.cameraPosition = transform->position;
            // TODO only primary Camera for now
            break;
        }
    }
    // DIR_LIGHT_MOVEMENT
    for (Entity entity : registry.view<DirLightMovementComponent>()) {
        auto* dirLightMovement = registry.getComponent<DirLightMovementComponent>(entity);

        if (dirLightMovement->isPrimary) {
            m_renderContext.hasDirLightMovement = true;
            m_renderContext.dirLightMovementDirection = dirLightMovement->direction;
            m_renderContext.dirLightMovementColor = dirLightMovement->color;
            // TODO only primary Directional Light for now
            break;
        }
    }

    // PHYSICS
    for (Entity entity : registry.view<TransformComponent, PhysicsComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* physics = registry.getComponent<PhysicsComponent>(entity);

        glm::vec3 AABBSize = Utils::Math::calculateSize(physics->AABB.worldMin, physics->AABB.worldMax);
        glm::vec3 AABBCenter = Utils::Math::calculateCenter(physics->AABB.worldMin, physics->AABB.worldMax);
        glm::vec3 color = physics->isColliding ? Constants::Color::RED : Constants::Color::GREEN;
        RenderImmediateCommand command = {
                transform->position,
                transform->rotation,
                transform->scale,
                AABBSize,
                AABBCenter,
                color
        };
        m_renderImmediateCommands.push_back(command);
    }

    // RENDER
    for (Entity entity : registry.view<TransformComponent, RenderComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* render = registry.getComponent<RenderComponent>(entity);

        glm::mat4 modelMatrix = Utils::Math::calculateInterpolatedModelMatrix(transform->position,
                                                                              transform->prevPosition,
                                                                              transform->rotation,
                                                                              transform->prevRotation,
                                                                              transform->scale,
                                                                              transform->prevScale,
                                                                              alpha);
        glm::mat3 normalMatrix = Utils::Math::calculateNormalMatrix(modelMatrix);
        glm::vec3 interPosition = Utils::Math::calculateInterpolatedPosition(transform->position,
                                                                             transform->prevPosition,
                                                                             alpha);
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
    //       ▄████████    ▄████████ ███▄▄▄▄   ████████▄     ▄████████    ▄████████      
    //      ███    ███   ███    ███ ███▀▀▀██▄ ███   ▀███   ███    ███   ███    ███      
    //      ███    ███   ███    █▀  ███   ███ ███    ███   ███    █▀    ███    ███      
    //     ▄███▄▄▄▄██▀  ▄███▄▄▄     ███   ███ ███    ███  ▄███▄▄▄      ▄███▄▄▄▄██▀      
    //    ▀▀███▀▀▀▀▀   ▀▀███▀▀▀     ███   ███ ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀        
    //    ▀███████████   ███    █▄  ███   ███ ███    ███   ███    █▄  ▀███████████      
    //      ███    ███   ███    ███ ███   ███ ███   ▄███   ███    ███   ███    ███      
    //      ███    ███   ██████████  ▀█   █▀  ████████▀    ██████████   ███    ███      
    //      ███    ███                                                  ███    ███      
    // 
    //       ▄███████▄    ▄████████    ▄████████    ▄████████    ▄████████    ▄████████ 
    //      ███    ███   ███    ███   ███    ███   ███    ███   ███    ███   ███    ███ 
    //      ███    ███   ███    ███   ███    █▀    ███    █▀    ███    █▀    ███    █▀  
    //      ███    ███   ███    ███   ███          ███         ▄███▄▄▄       ███        
    //    ▀█████████▀  ▀███████████ ▀███████████ ▀███████████ ▀▀███▀▀▀     ▀███████████ 
    //      ███          ███    ███          ███          ███   ███    █▄           ███ 
    //      ███          ███    ███    ▄█    ███    ▄█    ███   ███    ███    ▄█    ███ 
    //     ▄████▀        ███    █▀   ▄████████▀   ▄████████▀    ██████████  ▄████████▀  
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
    //     ▄██████▄     ▄███████▄    ▄████████ ████████▄   ███    █▄     ▄████████ 
    //    ███    ███   ███    ███   ███    ███ ███    ███  ███    ███   ███    ███ 
    //    ███    ███   ███    ███   ███    ███ ███    ███  ███    ███   ███    █▀  
    //    ███    ███   ███    ███   ███    ███ ███    ███  ███    ███  ▄███▄▄▄     
    //    ███    ███ ▀█████████▀  ▀███████████ ███    ███  ███    ███ ▀▀███▀▀▀     
    //    ███    ███   ███          ███    ███ ███    ███  ███    ███   ███    █▄  
    //    ███    ███   ███          ███    ███ ███  ▀ ███  ███    ███   ███    ███ 
    //     ▀██████▀   ▄████▀        ███    █▀   ▀██████▀▄█ ████████▀    ██████████ 
    _sortQueueByMaterial(m_opaqueQueue);
    _renderSortedQueue(m_opaqueQueue, "opaque pass", m_renderContext.cameraProjection);

    //       ▄████████     ███        ▄████████ ███▄▄▄▄    ▄████████  ▄█   ▄█       
    //      ███    ███ ▀█████████▄   ███    ███ ███▀▀▀██▄ ███    ███ ███  ███       
    //      ███    █▀     ▀███▀▀██   ███    █▀  ███   ███ ███    █▀  ███▌ ███       
    //      ███            ███   ▀  ▄███▄▄▄     ███   ███ ███        ███▌ ███       
    //    ▀███████████     ███     ▀▀███▀▀▀     ███   ███ ███        ███▌ ███       
    //             ███     ███       ███    █▄  ███   ███ ███    █▄  ███  ███       
    //       ▄█    ███     ███       ███    ███ ███   ███ ███    ███ ███  ███▌    ▄ 
    //     ▄████████▀     ▄████▀     ██████████  ▀█   █▀  ████████▀  █▀   █████▄▄██ 
    //                                                                    ▀         
    if (!m_stencilQueue.empty()) {
        m_renderer->stencilPass();
        _sortQueueByMaterial(m_stencilQueue);
        _renderSortedQueue(m_stencilQueue, "stencil pass", m_renderContext.cameraProjection);
    }

    //     ▄██████▄  ███    █▄      ███      ▄█        ▄█  ███▄▄▄▄      ▄████████ 
    //    ███    ███ ███    ███ ▀█████████▄ ███       ███  ███▀▀▀██▄   ███    ███ 
    //    ███    ███ ███    ███    ▀███▀▀██ ███       ███▌ ███   ███   ███    █▀  
    //    ███    ███ ███    ███     ███   ▀ ███       ███▌ ███   ███  ▄███▄▄▄     
    //    ███    ███ ███    ███     ███     ███       ███▌ ███   ███ ▀▀███▀▀▀     
    //    ███    ███ ███    ███     ███     ███       ███  ███   ███   ███    █▄  
    //    ███    ███ ███    ███     ███     ███▌    ▄ ███  ███   ███   ███    ███ 
    //     ▀██████▀  ████████▀     ▄████▀   █████▄▄██ █▀    ▀█   █▀    ██████████ 
    //                                      ▀                                     
    if (!m_outlineQueue.empty()) {
        m_renderer->outlinePass();
        _sortQueueByMaterial(m_outlineQueue);
        _renderSortedQueue(m_outlineQueue, "outline pass", m_renderContext.cameraProjection);
        m_renderer->outlinePass(false);
    }

    //    ▀█████████▄   ▄█          ▄████████ ███▄▄▄▄   ████████▄   ▄█  ███▄▄▄▄      ▄██████▄  
    //      ███    ███ ███         ███    ███ ███▀▀▀██▄ ███   ▀███ ███  ███▀▀▀██▄   ███    ███ 
    //      ███    ███ ███         ███    █▀  ███   ███ ███    ███ ███▌ ███   ███   ███    █▀  
    //     ▄███▄▄▄██▀  ███        ▄███▄▄▄     ███   ███ ███    ███ ███▌ ███   ███  ▄███        
    //    ▀▀███▀▀▀██▄  ███       ▀▀███▀▀▀     ███   ███ ███    ███ ███▌ ███   ███ ▀▀███ ████▄  
    //      ███    ██▄ ███         ███    █▄  ███   ███ ███    ███ ███  ███   ███   ███    ███ 
    //      ███    ███ ███▌    ▄   ███    ███ ███   ███ ███   ▄███ ███  ███   ███   ███    ███ 
    //    ▄█████████▀  █████▄▄██   ██████████  ▀█   █▀  ████████▀  █▀    ▀█   █▀    ████████▀  
    //                 ▀                                                                       
    if (!m_blendingQueue.empty()) {
        m_renderer->blendingPass();
        _sortQueueByDistance(m_blendingQueue);
        _renderSortedQueue(m_blendingQueue, "blending pass", m_renderContext.cameraProjection);
        m_renderer->blendingPass(false);
    }

    //        ███      ▄██████▄     ▄███████▄                        
    //    ▀█████████▄ ███    ███   ███    ███                        
    //       ▀███▀▀██ ███    ███   ███    ███                        
    //        ███   ▀ ███    ███   ███    ███                        
    //        ███     ███    ███ ▀█████████▀                         
    //        ███     ███    ███   ███                               
    //        ███     ███    ███   ███                               
    //       ▄████▀    ▀██████▀   ▄████▀                             
    //                                                               
    //     ▄█          ▄████████ ▄██   ▄      ▄████████    ▄████████ 
    //    ███         ███    ███ ███   ██▄   ███    ███   ███    ███ 
    //    ███         ███    ███ ███▄▄▄███   ███    █▀    ███    ███ 
    //    ███         ███    ███ ▀▀▀▀▀▀███  ▄███▄▄▄      ▄███▄▄▄▄██▀ 
    //    ███       ▀███████████ ▄██   ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   
    //    ███         ███    ███ ███   ███   ███    █▄  ▀███████████ 
    //    ███▌    ▄   ███    ███ ███   ███   ███    ███   ███    ███ 
    //    █████▄▄██   ███    █▀   ▀█████▀    ██████████   ███    ███ 
    //    ▀                                               ███    ███ 
    if (!m_topLayerQueue.empty()) {
        // always last (before UI)
        m_renderer->topLayerPass();
        glm::mat4 topLayerProjection = Utils::Math::calculateProjection(
            45.0f,
            m_renderContext.cameraAspect,
            0.01f,
            10.0f
        );
        _sortQueueByMaterial(m_topLayerQueue);
        _renderSortedQueue(m_topLayerQueue, "top layer pass", topLayerProjection);
    }

    //    ███    █▄   ▄█  
    //    ███    ███ ███  
    //    ███    ███ ███▌ 
    //    ███    ███ ███▌ 
    //    ███    ███ ███▌ 
    //    ███    ███ ███  
    //    ███    ███ ███  
    //    ████████▀  █▀   
    //                    
    if (!m_uiQueue.empty()) {
        // always last (last last)
        _sortQueueByMaterial(m_uiQueue);
        _renderSortedQueue(m_uiQueue, "ui pass", m_renderContext.cameraProjection);
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
    glm::vec3 cameraPosition = m_renderContext.cameraPosition;
    std::sort(queue.begin(), queue.end(), [cameraPosition](const RenderCommand& cmd1, const RenderCommand& cmd2) {
        return glm::length(cameraPosition - cmd2.position) < glm::length(cameraPosition - cmd1.position);
    });
}

// TODO: use UBO
void RenderSystem::_renderSortedQueue(std::vector<RenderCommand>& queue, const std::string& name, const glm::mat4& projection) const {
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
            const std::string& lastMN = (lastMaterial != nullptr) ? lastMaterial->getName() : "NULL";
            //LOG_D("Switching material from: " << lastMN << " to: " << currMaterial->getName());
            Shader* currShader = currMaterial->getShader();
            if (currShader != lastShader) {
                unsigned int lastSI = (lastShader != nullptr) ? lastShader->getID() : 999;
                //LOG_D("Switching shader from: " << lastSI << " to: " << currShader->getID());
                currShader->use();

                // draws per-shader (rarely)
                currShader->setMat4fv("view", m_renderContext.cameraView);
                currShader->setMat4fv("projection", projection);
                currShader->setVec3fv("viewPos", cmd.position);
                if (m_renderContext.hasDirLightMovement) {
                    currShader->setVec3fv("lightDir", m_renderContext.dirLightMovementDirection);
                    currShader->setVec3fv("lightColor", m_renderContext.dirLightMovementColor);
                }
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
    unsigned int VAO{};
    std::vector<RenderImmediateCommand> queue;

    switch (m_renderDebugMode) {
    case RenderDebugMode::NONE:
        return;
    case RenderDebugMode::AABB:
        queue = m_renderImmediateCommands;
        if (queue.empty()) {
            LOG_D("nothing to render immediately, sad QQ");
            return;
        }
        VAO = m_renderer->getVAOAABB();
        break;
    }

    // FIXME: m_renderer!
    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VAO);

    auto shader = ResourceManager::getInstance().getShader("simple_shader");
    shader->use();
    shader->setMat4fv("view", m_renderContext.cameraView);
    shader->setMat4fv("projection", m_renderContext.cameraProjection);
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

    m_renderImmediateCommands.clear();
}

void RenderSystem::endFrame(GLFWwindow* window) {
    // FIXME: cant use GLFWwindow here!
    m_renderer->endFrame(window);
}

void RenderSystem::toggleRasterizationMode() {
    m_renderer->toggleRasterizationMode();
}

void RenderSystem::toggleRenderDebugMode() {
    m_renderDebugMode = Utils::Enum::getNext(m_renderDebugMode);
    LOG_D("Changed RenderDebugMode to: " << Utils::Enum::getName(m_renderDebugMode));
}