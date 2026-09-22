#include "../../configs/gl_config.hpp"
#include "../../configs/log_config.hpp"
#include "../../ecs/registry.h"
#include "../../ecs/systems/camera_system.h"
#include "../../ecs/systems/render_system.h"
#include "../../utils/enum_utils.hpp"
#include "../i_backend.h"
#include "../i_renderer.h"
#include "opengl_renderer.h"

OpenGLRenderer::~OpenGLRenderer() {
    glDeleteVertexArrays(1, &m_VAOAABB);
    glDeleteBuffers(1, &m_VBOAABB);
}

void OpenGLRenderer::init() {
    // FIXME: move to new AABBMesh()
    // hardcoded AABB 1x1x1 (with the middle at 0.0)
    float verticesAABB[] = {
        // front
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        // back
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        // connectors
        -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };
    glGenVertexArrays(1, &m_VAOAABB);
    glGenBuffers(1, &m_VBOAABB);
    glBindVertexArray(m_VAOAABB);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOAABB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAABB), verticesAABB, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // FRAMEBUFFER
    //unsigned int fbo{};
    glGenFramebuffers(1, &m_framebuffer);
    // off-screen rendering
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    //unsigned int texture{};
    glGenTextures(1, &m_fbTexture);
    glBindTexture(GL_TEXTURE_2D, m_fbTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // target, attachment, textarget, texture, level
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbTexture, 0);

    // renderbuffer
    unsigned int rbo{};
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1280, 768);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // attach renderbuffer to framebuffer's depth and stencil attachments
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_E("RENDER_SYSTEM::FRAMEBUFFER_NOT_COMPLETE");
    }

    // main window rendering
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glDeleteFramebuffers(1, &m_framebuffer);

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
    unsigned int quadVBO;
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void OpenGLRenderer::beginFrame(int x, int y, int width, int height, const RendererState& state) {
    // TODO: add caching
    // Viewport
    glViewport(x, y, width, height);

    // Z-Depth Test
    if (state.depthTest) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    // Face Culling
    if (state.faceCulling) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
    } else {
        glDisable(GL_CULL_FACE);
    }
    // Scissor Test
    if (state.scissorTest) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(x, y, width, height);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }

    // Clear Buffers
    GLbitfield clearFlags = 0;
    if (state.clearColorBB) {
        clearFlags |= GL_COLOR_BUFFER_BIT;
    }
    if (state.clearDepthBB) {
        clearFlags |= GL_DEPTH_BUFFER_BIT;
    }
    if (state.clearStencilBB) {
        clearFlags |= GL_STENCIL_BUFFER_BIT;
    }
    if (clearFlags != 0) {
        glClearColor(state.clearColor.r, state.clearColor.g, state.clearColor.b, state.clearColor.a);
        glClear(clearFlags);
    }
}

void OpenGLRenderer::endFrame(GLFWwindow* window) {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_STENCIL_TEST);
    //glDisable(GL_BLEND);

    glfwSwapBuffers(window);

    //MINIMAP
    //glDisable(GL_SCISSOR_TEST);
}


void OpenGLRenderer::stencilPass() {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void OpenGLRenderer::outlinePass(bool start) {
    if (start) {
        glDisable(GL_DEPTH_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);
    } else {
        glEnable(GL_DEPTH_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
        glStencilMask(0xFF);
        glDisable(GL_STENCIL_TEST);
    }
}

void OpenGLRenderer::blendingPass(bool start) {
    if (start) {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        // src: factor == source color vector, dst: factor == 1 - source color vector
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    } else {
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
}

void OpenGLRenderer::topLayerPass() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::setRasterizationMode() {
    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(m_rasterizationMode));
}

void OpenGLRenderer::toggleRasterizationMode() {
    m_rasterizationMode = Utils::Enum::getNext(m_rasterizationMode);
    setRasterizationMode();
    LOG_D("Changed polygon rasterization mode to: " << Utils::Enum::getName(m_rasterizationMode));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // crazy static BS
    IBackEnd* backEnd = static_cast<IBackEnd*>(glfwGetWindowUserPointer(window));
    backEnd->getCameraSystem().updateAspect(backEnd->getRegistry(), width, height);
    backEnd->setViewport(0, 0, width, height);
}