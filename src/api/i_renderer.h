#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"

struct RendererState {
	bool depthTest{};
	bool scissorTest{};
	bool faceCulling{};
	glm::vec4 clearColor{};
	bool clearColorBB{};
	bool clearDepthBB{};
	bool clearStencilBB{};
};

class IRenderer {
public:
    virtual ~IRenderer();

	virtual void init() = 0;
	virtual void beginFrame(int x, int y, int width, int height, const RendererState& state) = 0;
	virtual void endFrame(GLFWwindow* window) = 0;
	virtual void stencilPass() = 0;
	virtual void outlinePass(bool start = true) = 0;
	virtual void blendingPass(bool start = true) = 0;
	virtual void topLayerPass() = 0;
	virtual void setRasterizationMode() = 0;
	virtual void toggleRasterizationMode() = 0;
	virtual unsigned int getVAOAABB() const = 0;
};