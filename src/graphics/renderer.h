#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"

enum class RenderMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

struct RenderLight {
	glm::vec3 direction;
	glm::vec3 color;
};

class Renderer {
public:
	Renderer(GLFWwindow* window);

	bool init();
	void toggleRenderMode();
	void beginFrame();
	void endFrame();

	RenderLight* getRenderLight() { return &m_renderLight; }
	void setLightDir(glm::vec3 lightDir) { m_renderLight.direction = lightDir; }

private:
	GLFWwindow* m_window = nullptr;
	RenderMode m_renderMode = RenderMode::STANDARD;
	RenderLight m_renderLight{};

	// must be static to be passed as a callback reference and so it needs to use core->var
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
};