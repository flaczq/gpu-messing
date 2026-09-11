#pragma once

#include "../../configs/gl_config.hpp"
#include <vector>

class Registry;
class RenderComponent;

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

	RenderMode m_renderMode{};

	void registerInQueue(RenderQueueType queueType, const RendererCommand& command);
};