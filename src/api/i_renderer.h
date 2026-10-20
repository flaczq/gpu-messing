#pragma once

class Mesh;
class FrameBuffer;

class IRenderer {
public:
    virtual ~IRenderer();

	virtual void init() = 0;
	virtual void beginFrame(int x, int y, int width, int height) = 0;
	virtual void beginFrameMinimap(int x, int y, int width, int height) = 0;
	virtual void stencilPass() = 0;
	virtual void outlinePass(bool start = true) = 0;
	virtual void blendingPass(bool start = true) = 0;
	virtual void topLayerPass() = 0;
	virtual void endFrame() = 0;
	virtual void endFrameMinimap() = 0;
	virtual void renderImmediate(bool start = true) = 0;
	virtual void setRasterizationMode() = 0;
	virtual void toggleRasterizationMode() = 0;
	virtual Mesh* getAABBMesh() = 0;
	virtual FrameBuffer* getScreenFrameBuffer() = 0;
};