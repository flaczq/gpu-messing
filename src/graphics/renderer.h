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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Renderer {
public:
	// Meyer's Singleton
	static Renderer& getInstance();
	Renderer(const Renderer&) = delete;
	void operator=(const Renderer&) = delete;

	bool init(GLFWwindow* window);
	void toggleRenderMode();
	void beginFrame();
	void endFrame();

	RenderLight* getRenderLight() { return &m_renderLight; }
	void setLightDir(glm::vec3 lightDir) { m_renderLight.direction = lightDir; }

private:
	// hidden constructor
	Renderer();

	GLFWwindow* m_window = nullptr;
	RenderMode m_renderMode = RenderMode::STANDARD;
	RenderLight m_renderLight{};
};