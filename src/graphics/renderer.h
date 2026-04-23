#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"

class Model;
class Material;

enum class RenderMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

struct RendererLight {
	glm::vec3 direction;
	glm::vec3 color;
};

struct RendererCommand {
	Model* model;
	Material* material;
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 viewPos;
	glm::mat4 modelMatrix;
	glm::mat3 normalMatrix;
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
	void drawStandard(const RendererCommand& command);
	void drawWithStencilWrite(const RendererCommand& command);
	void drawStencilOutline(const RendererCommand& command);
	void endFrame();

	RendererLight* getRendererLight() { return &m_rendererLight; }
	void setLightDir(glm::vec3 lightDir) { m_rendererLight.direction = lightDir; }

private:
	// hidden constructor
	Renderer();

	GLFWwindow* m_window = nullptr;
	RenderMode m_renderMode = RenderMode::STANDARD;
	RendererLight m_rendererLight{};

	void draw(const RendererCommand& command) const;
};