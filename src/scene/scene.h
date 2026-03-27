#pragma once

#include "../utils/commongl.h"

enum class SceneID {
	SOLDIER,
	LIGHTS_ROOM,
	FPS_GAME
};

class Scene {
public:
	virtual void init(GLFWwindow* window) = 0;
	virtual void update(GLFWwindow* window, float dt) = 0;
	virtual void render() = 0;
	virtual void end() = 0;

	virtual SceneID getSceneID() = 0;
	virtual Camera& getCamera() = 0;
};