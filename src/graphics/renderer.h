#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include <vector>

class Camera;
class Model;
class Material;

enum class RendererRenderMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};
enum class RendererQueueType {
	FIRST,
	STENCIL,
	OUTLINE
};

struct RendererLight {
	glm::vec3 direction;
	glm::vec3 color;
};
struct RendererCommand {
	Model* model;
	Material* material;
	//glm::mat4 projection;
	//glm::mat4 view;
	//glm::vec3 viewPos;
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

	bool init(GLFWwindow* window, Camera* camera);
	void toggleRenderMode();
	void beginFrame();
	void registerInQueue(RendererQueueType queueType, const RendererCommand& command);
	void flush();
	void endFrame();

	RendererLight* getRendererLight() { return &m_rendererLight; }
	void setLightDir(glm::vec3 lightDir) { m_rendererLight.direction = lightDir; }

private:
	// hidden constructor
	Renderer();

	GLFWwindow* m_window = nullptr;
	Camera* m_camera = nullptr;
	RendererRenderMode m_renderMode = RendererRenderMode::STANDARD;
	RendererLight m_rendererLight{};

	std::vector<RendererCommand> m_firstQueue;
	std::vector<RendererCommand> m_stencilQueue;
	std::vector<RendererCommand> m_outlineQueue;

	void renderSortedQueue(std::vector<RendererCommand>& queue, const std::string& name) const;
};