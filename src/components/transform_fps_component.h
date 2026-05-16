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

private:
	Camera* m_camera = nullptr;

	float m_sway{};
	float m_prevSway{};
};