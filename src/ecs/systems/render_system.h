#pragma once

#include "../../utils/commongl.h"
#include "camera_system.h"

class RenderSystem {
public:
	void update(entt::registry registry, float screenWidth, float screenHeight);
};