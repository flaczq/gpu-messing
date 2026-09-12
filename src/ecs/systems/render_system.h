#pragma once

#include "../../configs/gl_config.hpp"
#include "../../configs/math_config.hpp"
#include "../components/render_component.hpp"
#include <vector>

enum class RenderMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};
enum class RenderDebugMode {
	NONE,
	AABB
};

struct RenderContext {
	CameraComponent* mainCamera;
	TransformComponent* mainCameraTransform;
};
struct RenderCommand {
	Model* model;
	Material* material;
	glm::mat4 modelMatrix;
	glm::mat3 normalMatrix;
	glm::vec3 position;
};
struct RenderImmediateCommand {
	unsigned int VAO;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	glm::vec3 size;
	glm::vec3 center;
	glm::vec3 color;
};
struct RendererLight {
	glm::vec3 direction;
	glm::vec3 color;
};

class Registry;
class TransformComponent;
class CameraComponent;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class RenderSystem {
public:
	RenderSystem();

	bool init(GLFWwindow* window);
	void beginFrame(unsigned int width, unsigned int height);
	void beginFrameMinimap(unsigned int minimapWidth, unsigned int minimapHeight);
	void update(Registry& registry, float alpha);
	void execute();
	void renderImmediate();
	void endFrame();
	void endFrameMinimap();
	void toggleRenderMode();
	void toggleRenderDebugMode();

private:
	GLFWwindow* m_window{};
	std::vector<RenderCommand> m_opaqueQueue{};
	std::vector<RenderCommand> m_stencilQueue{};
	std::vector<RenderCommand> m_outlineQueue{};
	std::vector<RenderCommand> m_blendingQueue{};
	std::vector<RenderCommand> m_topLayerQueue{};
	std::vector<RenderCommand> m_uiQueue{};

	RenderMode m_renderMode = RenderMode::STANDARD;
	RenderDebugMode m_renderDebugMode = RenderDebugMode::NONE;
	RenderContext m_renderContext{};

	void _registerInQueue(RenderQueueType queueType, const RenderCommand& command);
	void _sortQueueByMaterial(std::vector<RenderCommand>& queue) const;
	void _sortQueueByDistance(std::vector<RenderCommand>& queue) const;
	void _renderSortedQueue(std::vector<RenderCommand>& queue, const std::string& name, const glm::mat4& projection) const;
	void _renderFrameBufferTexture();
};