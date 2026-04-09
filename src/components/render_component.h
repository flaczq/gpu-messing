#pragma once

#include "../configs/math_config.hpp"
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
	RenderComponent(std::shared_ptr<Model> model, std::shared_ptr<Shader> shader);

	void onInit() override;
	void draw(float alpha, RenderContext ctx);

	Model* getModel() const { return m_model.get(); }
	Shader* getShader() const { return m_shader.get(); }

private:
	std::shared_ptr<Model> m_model;
	std::shared_ptr<Shader> m_shader;

	TransformComponent* m_transform = nullptr;
};