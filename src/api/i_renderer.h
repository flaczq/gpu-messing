#pragma once

enum class RenderState {
	BEFORE,
	THIS,
	AFTER
};

class IRenderer {
public:
    virtual ~IRenderer();

	virtual void init(int screenWidth, int screenHeight) = 0;
	virtual void beginFrame(int x, int y, int width, int height) = 0;
	virtual void beginFrameMinimap(int x, int y, int minimapWidth, int minimapHeight) = 0;
	virtual void render(RenderState state) = 0;
	virtual void stencilPass() = 0;
	virtual void outlinePass(RenderState state) = 0;
	virtual void blendingPass(RenderState state) = 0;
	virtual void singlesPass(RenderState state) = 0;
	virtual void topLayerPass() = 0;
	virtual void endFrame() = 0;
	virtual void endFrameMinimap() = 0;
	virtual void renderImmediate(RenderState state) = 0;
	virtual void setRasterizationMode() = 0;
	virtual void toggleRasterizationMode() = 0;
};