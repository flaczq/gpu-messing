#include "../configs/log_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include "component.h"
#include "render_component.h"
#include "transform_component.h"
#include <memory>

RenderComponent::RenderComponent(std::shared_ptr<Model> model, std::shared_ptr<Material> material)
	: m_model(std::move(model)),
	  m_material(std::move(material))
{
}

void RenderComponent::onInit() {
	m_transform = getOwner()->getTransform();
}

void RenderComponent::onUpdate(float alpha) const {
    glm::mat4 modelMatrix = m_transform->getInterpolatedModelMatrix(alpha);
    glm::mat3 normalMatrix = m_transform->getNormalMatrix();
    glm::vec3 position = m_transform->getPosition();
    m_transform->setDirty(false);
    RendererQueueType queueType = getOwner()->getRendererQueueType();
    RendererCommand command = {
        m_model.get(),
        m_material.get(),
        modelMatrix,
        normalMatrix,
        position
    };
    Renderer::getInstance().registerInQueue(queueType, command);
}