#pragma once

#include "../configs/math_config.hpp"
#include "component.h"

class Camera;
class TransformComponent;

class TransformFpsComponent : public Component {
public:
	TransformFpsComponent(Camera* camera);

	void onInit() override;

	glm::mat4 getInterpolatedModelMatrix(float alpha);

private:
	Camera* m_camera = nullptr;
	TransformComponent* m_transform = nullptr;
};