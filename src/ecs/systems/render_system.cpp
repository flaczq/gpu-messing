#include "../entites/entity.hpp"
#include "../registry.h"
#include "render_system.h"

RenderSystem::RenderSystem() = default;

void RenderSystem::update(Registry& registry, float alpha) {
    glm::mat4 modelMatrix = m_transform->getInterpolatedModelMatrix(alpha);
    glm::mat3 normalMatrix = m_transform->getNormalMatrix();
    glm::vec3 interPosition = m_transform->getInterpolatedPosition(alpha);
    //m_transform->setDirty(false);
    RendererQueueType queueType = getOwner()->getRendererQueueType();
    RendererCommand command = {
        m_model.get(),
        m_material.get(),
        modelMatrix,
        normalMatrix,
        interPosition
    };
    Renderer::getInstance().registerInQueue(queueType, command);
}