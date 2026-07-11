#pragma once

#include "../configs/math_config.hpp"
#include "transform_component.h"

class Camera;

class TransformFpsComponent : public TransformComponent {
public:
	TransformFpsComponent(Camera* camera);

	void saveState() override;
	void onFixedUpdate(float fixedt) override;
	glm::mat4 getInterpolatedModelMatrix(float alpha) override;
	glm::vec3 getPosition() const override;

private:
	static constexpr glm::vec3 SWAY_OFFSET = glm::vec3(10.0f, 0.0f, 2.0f);

	Camera* m_camera = nullptr;

	float m_sway{};
	float m_prevSway{};
};