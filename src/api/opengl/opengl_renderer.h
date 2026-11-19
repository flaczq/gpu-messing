#pragma once

#include "../../configs/gl_config.hpp"
#include "../i_renderer.h"
#include <memory>

enum class RasterizationMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

// FIXME configuration
struct RenderConfiguration {
	bool postProcessing = false;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Mesh;
class FrameBuffer;
class Shader;
class CubeMap;

class OpenGLRenderer : public IRenderer {
public:
	OpenGLRenderer(GLFWwindow*& windowRef);

	void init(int screenWidth, int screenHeight) override;
	void beginFrame(int x, int y, int width, int height) override;
	void beginFrameMinimap(int x, int y, int minimapWidth, int minimapHeight) override;
	void render(RenderState state) override;
	void stencilPass() override;
	void outlinePass(RenderState state) override;
	void blendingPass(RenderState state) override;
	void topLayerPass() override;
	void endFrame() override;
	void endFrameMinimap() override;
	void renderImmediate(RenderState state) override;
	void setRasterizationMode() override;
	void toggleRasterizationMode() override;

private:
	// reference to pointer (!!)
	// have to do it like this because RenderSystem can't know about GLFWWindow
	GLFWwindow*& m_windowRef;

	RasterizationMode m_rasterizationMode = RasterizationMode::STANDARD;
	RenderConfiguration m_renderConfiguration{};

	std::unique_ptr<Mesh> m_aabbMesh{};
	std::unique_ptr<Mesh> m_framebufferMesh{};
	std::unique_ptr<Mesh> m_skyboxMesh{};
	std::unique_ptr<FrameBuffer> m_screenFbo{};
	std::shared_ptr<Shader> m_renderImmediateShader{};
	std::shared_ptr<Shader> m_screenTexShader{};
	std::shared_ptr<Shader> m_skyboxShader{};
};