#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/game_entity.h"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "component.h"
#include "render_component.h"
#include "transform_component.h"
#include <memory>
#include <utility>

RenderComponent::RenderComponent(std::shared_ptr<Model> model, std::shared_ptr<Material> material)
	: m_model(std::move(model)),
	  m_material(std::move(material))
{
}

void RenderComponent::onInit() {
	m_transform = getOwner()->getTransform();
}

/*void RenderComponent::draw(float alpha, const RenderContext& ctx) {
	// transformation matrix: clip = projectionM * viewM * modelM * local
	// 1. local * modelM            -> world
	// 2. world * viewM             -> space (lookAt())
	// 3. space * projectionM       -> clip
	// 4. clip  * viewportTransform -> screen
	glm::mat4 modelMatrix = m_transform->getInterpolatedModelMatrix(alpha);
	glm::mat3 normalMatrix = m_transform->getNormalMatrix(modelMatrix);
	m_transform->setDirty(false);

	auto* shader = m_material->getShader();
	shader->setMat4fv("projection", ctx.projection);
	shader->setMat4fv("view", ctx.view);
	shader->setVec3fv("viewPos", ctx.viewPos);
	shader->setMat4fv("model", modelMatrix);
	shader->setMat3fv("normalMatrix", normalMatrix);

	m_model->draw(*shader);
}*/