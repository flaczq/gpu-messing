#pragma once

#include "../utils/config.h"
#include "../core/camera.h"

class Scene {
public:
	virtual void init() = 0;
	virtual void update(float dt) = 0;
	virtual void render() = 0;
	virtual void end() = 0;

	SceneID sceneID;
};