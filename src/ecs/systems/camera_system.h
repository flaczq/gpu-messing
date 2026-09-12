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
	void logMainCameraPosition(Registry& registry);

private:
	void _processMouseScroll(CameraComponent* camera, float yOffset);
	void _processMouseMovement(TransformComponent* transform, float xOffset, float yOffset, bool clampPitch = true);
};