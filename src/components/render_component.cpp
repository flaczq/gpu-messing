#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../entities/game_entity.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "component.h"
#include "render_component.h"
#include <memory>
#include <utility>

RenderComponent::RenderComponent(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader)
	: m_model(std::move(model)),
	  m_shader(std::move(shader))
{
}

void RenderComponent::draw(float alpha) {
	if (!isActive()) {
		LOG_D("RenderComponent is not active");
		return;
	}

	glm::mat4 model = getOwner()->getTransform()->getInterpolatedMatrix(alpha);
	glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

	m_shader->setMat4fv("model", model);
	m_shader->setMat3fv("normalMatrix", normalMatrix);

	m_model->draw(*m_shader);
}