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

// not reqd
void RenderComponent::onInit() {
	m_transform = getOwner()->getTransform();
}