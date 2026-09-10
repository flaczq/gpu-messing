#pragma once

#include "../../configs/gl_config.hpp"

class Registry;
class CameraComponent;

class CameraSystem {
public:
	CameraSystem();

	void processInput(Registry& registry);
	void updateAspect(Registry& registry, int width, int height);
	void updateView(Registry& registry, float alpha);
	void updateProjection(Registry& registry, bool force = false);
	void restoreDefaultProjection(Registry& registry);

private:
	void processMouseScroll(CameraComponent* cameraComponent, float yOffset);
	void processMouseMovement(TransformComponent* transform, CameraComponent* cameraComponent, float xOffset, float yOffset, bool clampPitch = true);
};