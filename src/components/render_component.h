#pragma once

#include "../configs/math_config.hpp"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "component.h"
#include <memory>

struct RenderContext {
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 viewPos;
};

class TransformComponent;

class RenderComponent : public Component {
public:
	RenderComponent(std::shared_ptr<Model> model, std::shared_ptr<Material> material);

	void onInit() override;
	void draw(float alpha, RenderContext ctx);

	Model* getModel() const { return m_model.get(); }
	Material* getMaterial() const { return m_material.get(); }

private:
	std::shared_ptr<Model> m_model;
	std::shared_ptr<Material> m_material;

	TransformComponent* m_transform = nullptr;
};