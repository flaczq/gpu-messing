#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "frame_buffer.h"
#include "graphics_types.hpp"
#include <memory>
#include <vector>

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : m_FBO(other.m_FBO),
      m_RBO(other.m_RBO),
      m_quadVAO(other.m_quadVAO),
      m_quadVBO(other.m_quadVBO),
      m_texture(other.m_texture)
{
    other.m_FBO = 0;
    other.m_RBO = 0;
    other.m_quadVAO = 0;
    other.m_quadVBO = 0;
    other.m_texture = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
    if (this != &other) {
        m_FBO = other.m_FBO;
        m_RBO = other.m_RBO;
        m_quadVAO = other.m_quadVAO;
        m_quadVBO = other.m_quadVBO;
        m_texture = other.m_texture;

        other.m_FBO = 0;
        other.m_RBO = 0;
        other.m_quadVAO = 0;
        other.m_quadVBO = 0;
        other.m_texture = 0;
    }
    return *this;
}

FrameBuffer::FrameBuffer(std::vector<Vertex>& vertices, std::shared_ptr<Texture> texture) {
    _setupFrameBuffer();
}

FrameBuffer::~FrameBuffer() {
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteVertexArrays(1, &m_quadVBO);
    glDeleteRenderbuffers(1, &m_RBO);
    glDeleteFramebuffers(1, &m_FBO);
}

void FrameBuffer::_setupFrameBuffer() {
    glGenFramebuffers(1, &m_FBO);
    // off-screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_texture->id);
    glBindTexture(GL_TEXTURE_2D, m_texture->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        LOG_E("RENDER_SYSTEM::FRAMEBUFFER_NOT_COMPLETE");
    }

    // main window rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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