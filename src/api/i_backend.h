#pragma once

#include <memory>

enum class GraphicsAPI {
    OPENGL,
    VULKAN
};

class Registry;
class CameraSystem;

class IBackEnd {
public:
    virtual ~IBackEnd();

    virtual bool init(unsigned int width, unsigned int height) = 0;
    virtual void run() = 0;
    virtual Registry& getRegistry() = 0;
    virtual CameraSystem& getCameraSystem() = 0;
    virtual void setViewport(int x, int y, int width, int height) = 0;

    static std::unique_ptr<IBackEnd> create(GraphicsAPI api);
};