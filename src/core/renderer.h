#pragma once

#include "../utils/config.h"

class Renderer {
public:
	Renderer(GLFWwindow* window);

	bool init();
	void toggleRenderMode();
	void beginFrame();
	void endFrame();

	RenderMode renderMode = RenderMode::STANDARD;

private:
	GLFWwindow* window = nullptr;

	// must be static to be passed as a callback reference and so it needs to use core->var
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};