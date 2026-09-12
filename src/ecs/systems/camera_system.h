#pragma once

#include "../../configs/gl_config.hpp"

class Registry;
class CameraComponent;

class CameraSystem {
public:
	CameraSystem();

	void processInput(Registry& registry);
	void updateView(Registry& registry, float alpha);
	void updateProjection(Registry& registry);
	void updateAspect(Registry& registry, int width, int height);

private:
	void processMouseScroll(CameraComponent* camera, float yOffset);
	void processMouseMovement(TransformComponent* transform, float xOffset, float yOffset, bool clampPitch = true);
};