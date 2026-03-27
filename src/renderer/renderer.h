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

	bool init(GLFWwindow* window);
	RenderMode getRenderMode() const;
	void toggleRenderMode();
	void beginFrame() const;
	void endFrame() const;

private:
	GLFWwindow* window;
	RenderMode renderMode = RenderMode::STANDARD;

	// must be static to be passed as a callback reference and so it needs to use core->var
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};