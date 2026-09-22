#pragma once

#include "../../configs/gl_config.hpp"
#include "../i_renderer.h"

enum class RasterizationMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class OpenGLRenderer : public IRenderer {
public:
	OpenGLRenderer(GLFWwindow*& windowRef);
	~OpenGLRenderer() override;

	void init() override;
	void beginFrame(int x, int y, int width, int height) override;
	void beginFrameMinimap(int x, int y, int width, int height) override;
	void endFrame() override;
	void endFrameMinimap() override;
	void stencilPass() override;
	void outlinePass(bool start = true) override;
	void blendingPass(bool start = true) override;
	void topLayerPass() override;
	void setRasterizationMode() override;
	void toggleRasterizationMode() override;
	unsigned int getVAOAABB() const override { return m_VAOAABB; }
private:
	// reference to pointer (!!)
	// have to do it like this because RenderSystem can't now about GLFWWindow
	GLFWwindow*& m_windowRef;

	RasterizationMode m_rasterizationMode = RasterizationMode::STANDARD;
	// AABB
	unsigned int m_VAOAABB{};
	unsigned int m_VBOAABB{};
	// FRAMEBUFFER
	unsigned int m_framebuffer{};
	unsigned int m_fbTexture{};
	unsigned int m_quadVAO;
};