#pragma once

#include "../utils/commongl.h"

enum class RenderMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

class Renderer {
public:
	Renderer();
	~Renderer() = default;

	RenderMode getRenderMode() const;
	void setRenderMode(RenderMode n_renderMode);
private:
	RenderMode renderMode = RenderMode::STANDARD;
};