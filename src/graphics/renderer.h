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
	OPAQUE,
	STENCIL,
	OUTLINE,
	BLENDING,
	TOP_LAYER
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
	glm::vec3 position;
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
	void beginFrame(unsigned int screenWidth, unsigned int screenHeight);
	void registerInQueue(RendererQueueType queueType, const RendererCommand& command);
	void flush();
	void endFrame();
	void beginFrameMinimap(unsigned int minimapWidth, unsigned int minimapHeight);
	void endFrameMinimap();

	void setCamera(Camera* camera) { m_camera = camera; }
	//void setStencilReqd(bool stencilReqd) { m_stencilReqd = stencilReqd; }
	//void setBlendingReqd(bool blendingReqd) { m_blendingReqd = blendingReqd; }
	RendererLight* getRendererLight() { return &m_light; }
	void setLightDir(glm::vec3 lightDir) { m_light.direction = lightDir; }

private:
	// hidden constructor
	Renderer();

	GLFWwindow* m_window = nullptr;
	Camera* m_camera = nullptr;
	RendererRenderMode m_renderMode = RendererRenderMode::STANDARD;
	RendererLight m_light{};
	//bool m_stencilReqd = false;
	//bool m_blendingReqd = false;

	std::vector<RendererCommand> m_opaqueQueue;
	std::vector<RendererCommand> m_stencilQueue;
	std::vector<RendererCommand> m_outlineQueue;
	std::vector<RendererCommand> m_blendingQueue;
	std::vector<RendererCommand> m_topLayerQueue;

	void sortQueueByMaterial(std::vector<RendererCommand>& queue) const;
	void sortQueueByDistance(std::vector<RendererCommand>& queue) const;
	void renderSortedQueue(std::vector<RendererCommand>& queue, const std::string& name) const;
};