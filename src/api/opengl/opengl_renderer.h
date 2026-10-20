#pragma once

#include "../../configs/gl_config.hpp"
#include "../i_renderer.h"
#include <memory>

enum class RasterizationMode {
	STANDARD = GL_FILL,
	WIREFRAME = GL_LINE,
	POINTCLOUD = GL_POINT
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

class Mesh;
class FrameBuffer;

class OpenGLRenderer : public IRenderer {
public:
	OpenGLRenderer(GLFWwindow*& windowRef);
	~OpenGLRenderer() override;

	void init() override;
	void beginFrame(int x, int y, int width, int height) override;
	void beginFrameMinimap(int x, int y, int width, int height) override;
	void stencilPass() override;
	void outlinePass(bool start = true) override;
	void blendingPass(bool start = true) override;
	void topLayerPass() override;
	void endFrame() override;
	void endFrameMinimap() override;
	void renderImmediate(bool start = true) override;
	void setRasterizationMode() override;
	void toggleRasterizationMode() override;
	Mesh* getAABBMesh() override { return aabbMesh.get(); }
	FrameBuffer* getScreenFrameBuffer() override { return screenFbo.get(); }

	std::unique_ptr<Mesh> aabbMesh{};
	std::unique_ptr<Mesh> fboMesh{};
	std::unique_ptr<FrameBuffer> screenFbo{};
private:
	// reference to pointer (!!)
	// have to do it like this because RenderSystem can't know about GLFWWindow
	GLFWwindow*& m_windowRef;

	RasterizationMode m_rasterizationMode = RasterizationMode::STANDARD;
	// AABB
	unsigned int m_aabbVAO{};
	unsigned int m_aabbVBO{};
	unsigned int m_quadVAO{};
	unsigned int m_quadVBO{};
};