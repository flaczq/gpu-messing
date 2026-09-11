#include "../../configs/gl_config.hpp"
#include "../../graphics/renderer.h"
#include "../../utils/component_utils.hpp"
#include "../components/render_component.hpp"
#include "../components/transform_component.hpp"
#include "../entites/entity.hpp"
#include "../registry.h"
#include "render_system.h"

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

void RenderSystem::update(Registry& registry, float alpha) {
    for (Entity entity : registry.view<TransformComponent, RenderComponent>()) {
        auto* transform = registry.getComponent<TransformComponent>(entity);
        auto* render = registry.getComponent<RenderComponent>(entity);

        glm::mat4 modelMatrix = Utils::Component::calculateInterpolatedModelMatrix(*transform, alpha);
        glm::mat3 normalMatrix = Utils::Component::calculateNormalMatrix(modelMatrix);
        glm::vec3 interPosition = Utils::Component::calculateInterpolatedPosition(*transform, alpha);

        RenderQueueType queueType = render->queueType;
        RendererCommand command = {
            render->model.get(),
            render->material.get(),
            modelMatrix,
            normalMatrix,
            interPosition
        };
        registerInQueue(queueType, command);
    }
}

void RenderSystem::registerInQueue(RenderQueueType queueType, const RendererCommand& command) {
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