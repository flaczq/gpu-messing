#include "../../configs/gl_config.hpp"
#include "../../configs/log_config.hpp"
#include "../../ecs/registry.h"
#include "../../ecs/systems/camera_system.h"
#include "../../graphics/frame_buffer.h"
#include "../../graphics/mesh.h"
#include "../../graphics/mesh_generator.h"
#include "../../graphics/shader.h"
#include "../../managers/resource_manager.h"
#include "../../utils/enum_utils.hpp"
#include "../i_backend.h"
#include "../i_renderer.h"
#include "opengl_renderer.h"
#include <memory>

OpenGLRenderer::OpenGLRenderer(GLFWwindow*& windowRef)
    : m_windowRef(windowRef)
{
}

OpenGLRenderer::~OpenGLRenderer() {
    glDeleteVertexArrays(1, &m_aabbVAO);
    glDeleteBuffers(1, &m_aabbVBO);
}

void OpenGLRenderer::init() {
    //        ███        ▄████████ ▀████    ▐████▀     ███     ███    █▄     ▄████████    ▄████████    ▄████████ 
    //    ▀█████████▄   ███    ███   ███▌   ████▀  ▀█████████▄ ███    ███   ███    ███   ███    ███   ███    ███ 
    //       ▀███▀▀██   ███    █▀     ███  ▐███       ▀███▀▀██ ███    ███   ███    ███   ███    █▀    ███    █▀  
    //        ███   ▀  ▄███▄▄▄        ▀███▄███▀        ███   ▀ ███    ███  ▄███▄▄▄▄██▀  ▄███▄▄▄       ███        
    //        ███     ▀▀███▀▀▀        ████▀██▄         ███     ███    ███ ▀▀███▀▀▀▀▀   ▀▀███▀▀▀     ▀███████████ 
    //        ███       ███    █▄    ▐███  ▀███        ███     ███    ███ ▀███████████   ███    █▄           ███ 
    //        ███       ███    ███  ▄███     ███▄      ███     ███    ███   ███    ███   ███    ███    ▄█    ███ 
    //       ▄████▀     ██████████ ████       ███▄    ▄████▀   ████████▀    ███    ███   ██████████  ▄████████▀  
    //                                                                      ███    ███                           
    ResourceManager::getInstance().loadTexture("window_texture", "../assets/blending_transparent_window.png");
    ResourceManager::getInstance().loadTexture("grass_texture", "../assets/grass.png");
    ResourceManager::getInstance().loadTexture("potato_texture", "../assets/potato.jpg");
    ResourceManager::getInstance().loadTexture("container_texture", "../assets/container.jpg");
    //       ▄████████    ▄█    █▄       ▄████████ ████████▄     ▄████████    ▄████████    ▄████████ 
    //      ███    ███   ███    ███     ███    ███ ███   ▀███   ███    ███   ███    ███   ███    ███ 
    //      ███    █▀    ███    ███     ███    ███ ███    ███   ███    █▀    ███    ███   ███    █▀  
    //      ███         ▄███▄▄▄▄███▄▄   ███    ███ ███    ███  ▄███▄▄▄      ▄███▄▄▄▄██▀   ███        
    //    ▀███████████ ▀▀███▀▀▀▀███▀  ▀███████████ ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ▀███████████ 
    //             ███   ███    ███     ███    ███ ███    ███   ███    █▄  ▀███████████          ███ 
    //       ▄█    ███   ███    ███     ███    ███ ███   ▄███   ███    ███   ███    ███    ▄█    ███ 
    //     ▄████████▀    ███    █▀      ███    █▀  ████████▀    ██████████   ███    ███  ▄████████▀  
    //                                                                       ███    ███              
    ResourceManager::getInstance().loadShader("simple_shader", "../shaders/simple.vert", "../shaders/simple.frag");
    ResourceManager::getInstance().loadShader("gizmo_shader", "../shaders/gizmo.vert", "../shaders/gizmo.frag");
    ResourceManager::getInstance().loadShader("lambert_shader", "../shaders/lambert.vert", "../shaders/lambert.frag");
    ResourceManager::getInstance().loadShader("model_shader", "../shaders/model.vert", "../shaders/model.frag");
    ResourceManager::getInstance().loadShader("window_shader", "../shaders/window.vert", "../shaders/window.frag");
    ResourceManager::getInstance().loadShader("screen_tex_shader", "../shaders/screen_tex.vert", "../shaders/screen_tex.frag");

    // FIXME: move to new AABBMesh()
    // hardcoded AABB 1x1x1 (with the middle at 0.0)
    /*float aabbVertices[] = {
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
    glGenVertexArrays(1, &m_aabbVAO);
    glGenBuffers(1, &m_aabbVBO);
    glBindVertexArray(m_aabbVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_aabbVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(aabbVertices), aabbVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);*/

    // screen quad VAO
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         //
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glBindVertexArray(m_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)(2 * sizeof(float)));
    glBindVertexArray(0);

    auto screenTexShader = ResourceManager::getInstance().getShader("screen_tex_shader");
    screenTexShader->use();
    screenTexShader->setInt("screenTexture", 0);

    // AABB
    auto aabbM = MeshGenerator::createCuboid(1.0f, 1.0f, 1.0f);
    aabbMesh = std::make_unique<Mesh>(std::move(aabbM));

    // FRAMEBUFFER
    auto containerTexture = ResourceManager::getInstance().getTexture("container_texture");
    //auto fboM = MeshGenerator::createCuboid(2.0f, 2.0f, 0.0f, containerTexture);
    //fboMesh = std::make_unique<Mesh>(std::move(fboM));
    screenFbo = std::make_unique<FrameBuffer>(std::move(containerTexture));
}

void OpenGLRenderer::beginFrame(int x, int y, int width, int height) {
    // Viewport
    glViewport(x, y, width, height);

    screenFbo->bind();

    // Z-Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Clear Buffers
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRenderer::beginFrameMinimap(int x, int y, int width, int height) {
    glViewport(x, y, width, height);

    // Scissor Test
    glEnable(GL_SCISSOR_TEST);
    glScissor(x, y, width, height);

    glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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

void OpenGLRenderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    screenFbo->unbind();
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.1f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    auto screenTexShader = ResourceManager::getInstance().getShader("screen_tex_shader");
    screenTexShader->use();
    glBindVertexArray(m_quadVAO);
    glBindTexture(GL_TEXTURE_2D, screenFbo->m_textureColorBuffer->id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    //auto screenTexShader = ResourceManager::getInstance().getShader("screen_tex_shader");
    //fboMesh->draw(*screenTexShader);

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_STENCIL_TEST);
    //glDisable(GL_BLEND);

    assert(m_windowRef);
    glfwSwapBuffers(m_windowRef);
}

void OpenGLRenderer::endFrameMinimap() {
    glDisable(GL_SCISSOR_TEST);
}

void OpenGLRenderer::renderImmediate(bool start) {
    if (start) {
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(m_aabbVAO);
    } else {
        glBindVertexArray(0);
        glEnable(GL_DEPTH_TEST);
    }
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