#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "frame_buffer.h"
#include "graphics_types.hpp"
#include <memory>

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : m_FBO(other.m_FBO),
      m_RBO(other.m_RBO),
      m_textureColorBuffer(std::move(other.m_textureColorBuffer))
{
    other.m_FBO = 0;
    other.m_RBO = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
    if (this != &other) {
        m_FBO = other.m_FBO;
        m_RBO = other.m_RBO;
        m_textureColorBuffer = std::move(other.m_textureColorBuffer);

        other.m_FBO = 0;
        other.m_RBO = 0;
    }
    return *this;
}

FrameBuffer::FrameBuffer(std::shared_ptr<Texture> texture)
    : m_textureColorBuffer(std::move(texture))
{
    _setupFrameBuffer();
}

FrameBuffer::~FrameBuffer() {
    glDeleteRenderbuffers(1, &m_RBO);
    glDeleteFramebuffers(1, &m_FBO);
}

void FrameBuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
}

void FrameBuffer::unbind() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::_setupFrameBuffer() {
    glGenFramebuffers(1, &m_FBO);
    // off-screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    // target, attachment, textarget, texture, level
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureColorBuffer->id, 0);

    // renderbuffer
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 768);
    // attach renderbuffer to framebuffer's depth and stencil attachments
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_E("FRAME_BUFFER::FRAME_BUFFER_NOT_COMPLETE");
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}