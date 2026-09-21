#include "../configs/log_config.hpp"
#include "./opengl/opengl_backend.h"
#include "backend.h"
#include <memory>
#include <stdexcept>

std::unique_ptr<BackEnd> BackEnd::create(GraphicsAPI api) {
    switch (api) {
    case GraphicsAPI::OPENGL:
        LOG("*** OpenGL for Windows");
        return std::make_unique<OpenGLBackEnd>();

    case GraphicsAPI::VULKAN:
        LOG_E("BACKEND::VULKAN_WINDOWS_NOT_IMPLEMENTED..._YET");
        throw std::logic_error("Not implemented for Vulkan... yet");
    }
    return nullptr;
}