#pragma once

#include "../../configs/gl_config.hpp"
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

class Registry;
class TransformComponent;
class RenderComponent;
class CameraComponent;

class RenderSystem {
public:
	RenderSystem();

	bool init(GLFWwindow* window);
	void update(Registry& registry, float alpha);

private:
	GLFWwindow* m_window{};
	std::vector<RendererCommand> m_opaqueQueue{};
	std::vector<RendererCommand> m_stencilQueue{};
	std::vector<RendererCommand> m_outlineQueue{};
	std::vector<RendererCommand> m_blendingQueue{};
	std::vector<RendererCommand> m_topLayerQueue{};
	std::vector<RendererCommand> m_uiQueue{};

	RenderMode m_renderMode = RenderMode::STANDARD;
	RenderContext m_renderContext{};

	void registerInQueue(RenderQueueType queueType, const RendererCommand& command);
};