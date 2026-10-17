#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "frame_buffer.h"
#include "graphics_types.hpp"
#include <memory>

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : m_FBO(other.m_FBO),
      m_RBO(other.m_RBO),
      m_texture(std::move(other.m_texture))
{
    other.m_FBO = 0;
    other.m_RBO = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
    if (this != &other) {
        m_FBO = other.m_FBO;
        m_RBO = other.m_RBO;
        m_texture = std::move(other.m_texture);

        other.m_FBO = 0;
        other.m_RBO = 0;
    }
    return *this;
}

FrameBuffer::FrameBuffer(std::shared_ptr<Texture> texture)
    : m_texture(std::move(texture))
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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id, 0);

    // renderbuffer
    glGenRenderbuffers(1, &m_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 768);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // attach renderbuffer to framebuffer's depth and stencil attachments
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_E("FRAME_BUFFER::ERROR_NOT_COMPLETE");
    }

    // screen quad VAO
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}