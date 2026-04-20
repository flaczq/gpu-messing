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

void RenderComponent::draw(float alpha, RenderContext ctx) {
	auto* shader = m_material->getShader();
	glm::mat4 model = m_transform->getInterpolatedMatrix(alpha);
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

	shader->setMat4fv("projection", ctx.projection);
	shader->setMat4fv("view", ctx.view);
	shader->setVec3fv("viewPos", ctx.viewPos);
	shader->setMat4fv("model", model);
	shader->setMat3fv("normalMatrix", normalMatrix);

	m_model->draw(*shader);
}