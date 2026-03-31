#pragma once

#include "../configs/gl_config.hpp"

enum class RenderMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

class Renderer {
public:
	Renderer(GLFWwindow* window);

	bool init();
	void toggleRenderMode();
	void beginFrame();
	void endFrame();

private:
	GLFWwindow* m_window = nullptr;

	RenderMode m_renderMode = RenderMode::STANDARD;

	// must be static to be passed as a callback reference and so it needs to use core->var
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};