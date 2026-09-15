#pragma once

#include "../../configs/gl_config.hpp"
#include "../../configs/math_config.hpp"
#include "../components/camera_component.hpp"
#include "../components/dir_light_movement_component.hpp"
#include "../components/render_component.hpp"
#include "../components/transform_component.hpp"
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
	glm::mat4 cameraView{};
	glm::mat4 cameraProjection{};
	float cameraAspect{};
	glm::vec3 cameraPosition{};
	bool hasDirLightMovement{};
	glm::vec3 dirLightMovementDirection{};
	glm::vec3 dirLightMovementColor{};
};
struct RenderCommand {
	Model* model{};
	Material* material{};
	glm::mat4 modelMatrix{};
	glm::mat3 normalMatrix{};
	glm::vec3 position{};
};
struct RenderImmediateCommand {
	glm::vec3 position{};
	glm::quat rotation{};
	glm::vec3 scale{};
	glm::vec3 size{};
	glm::vec3 center{};
	glm::vec3 color{};
};

class Registry;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class RenderSystem {
public:
	RenderSystem();
	~RenderSystem();

	bool init();
	void beginFrame(unsigned int width, unsigned int height);
	void beginFrameMinimap(unsigned int minimapWidth, unsigned int minimapHeight);
	void update(Registry& registry, float alpha);
	void execute();
	void renderImmediate();
	void endFrame(GLFWwindow* window);
	void endFrameMinimap();
	void toggleRenderMode();
	void toggleRenderDebugMode();

private:
	GLFWwindow* m_window{};
	RenderContext m_renderContext{};
	std::vector<RenderCommand> m_opaqueQueue{};
	std::vector<RenderCommand> m_stencilQueue{};
	std::vector<RenderCommand> m_outlineQueue{};
	std::vector<RenderCommand> m_blendingQueue{};
	std::vector<RenderCommand> m_topLayerQueue{};
	std::vector<RenderCommand> m_uiQueue{};
	std::vector<RenderImmediateCommand> m_renderImmediateCommands{};
	unsigned int m_VAOAABB{};
	unsigned int m_VBOAABB{};

	RenderMode m_renderMode = RenderMode::STANDARD;
	RenderDebugMode m_renderDebugMode = RenderDebugMode::AABB;

	void _registerInQueue(RenderQueueType queueType, const RenderCommand& command);
	void _sortQueueByMaterial(std::vector<RenderCommand>& queue) const;
	void _sortQueueByDistance(std::vector<RenderCommand>& queue) const;
	void _renderSortedQueue(std::vector<RenderCommand>& queue, const std::string& name, const glm::mat4& projection) const;
	void _renderFrameBufferTexture();
};