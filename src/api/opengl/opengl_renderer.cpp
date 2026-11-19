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
#include <string>
#include <vector>

OpenGLRenderer::OpenGLRenderer(GLFWwindow*& windowRef)
    : m_windowRef(windowRef)
{
}

void OpenGLRenderer::init(int screenWidth, int screenHeight) {
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
    ResourceManager::getInstance().allocateTextureMemory("container_texture", "../assets/container.jpg", screenWidth, screenHeight);
    std::vector<std::string> skybox = {
        "../assets/skybox/back.jpg",   "../assets/skybox/front.jpg",
        "../assets/skybox/left.jpg",   "../assets/skybox/right.jpg",
        "../assets/skybox/bottom.jpg", "../assets/skybox/top.jpg"
    };
    ResourceManager::getInstance().loadCubeMapTexture("skybox_texture", skybox);

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
    ResourceManager::getInstance().loadShader("skybox_shader", "../shaders/skybox.vert", "../shaders/skybox.frag");

    m_renderImmediateShader = ResourceManager::getInstance().getShader("simple_shader");
    m_screenTexShader = ResourceManager::getInstance().getShader("screen_tex_shader");
    m_skyboxShader = ResourceManager::getInstance().getShader("skybox_shader");

    //       ▄████████    ▄████████ ▀█████████▄  ▀█████████▄  
    //      ███    ███   ███    ███   ███    ███   ███    ███ 
    //      ███    ███   ███    ███   ███    ███   ███    ███ 
    //      ███    ███   ███    ███  ▄███▄▄▄██▀   ▄███▄▄▄██▀  
    //    ▀███████████ ▀███████████ ▀▀███▀▀▀██▄  ▀▀███▀▀▀██▄  
    //      ███    ███   ███    ███   ███    ██▄   ███    ██▄ 
    //      ███    ███   ███    ███   ███    ███   ███    ███ 
    //      ███    █▀    ███    █▀  ▄█████████▀  ▄█████████▀  
    auto aabbM = MeshGenerator::createAABBCuboid(1.0f, 1.0f, 1.0f);
    aabbM.setDrawType(GL_LINES);
    m_aabbMesh = std::make_unique<Mesh>(std::move(aabbM));

    //       ▄████████    ▄████████    ▄████████   ▄▄▄▄███▄▄▄▄      ▄████████         
    //      ███    ███   ███    ███   ███    ███ ▄██▀▀▀███▀▀▀██▄   ███    ███         
    //      ███    █▀    ███    ███   ███    ███ ███   ███   ███   ███    █▀          
    //     ▄███▄▄▄      ▄███▄▄▄▄██▀   ███    ███ ███   ███   ███  ▄███▄▄▄             
    //    ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ▀███████████ ███   ███   ███ ▀▀███▀▀▀             
    //      ███        ▀███████████   ███    ███ ███   ███   ███   ███    █▄          
    //      ███          ███    ███   ███    ███ ███   ███   ███   ███    ███         
    //      ███          ███    ███   ███    █▀   ▀█   ███   █▀    ██████████         
    //                   ███    ███                                                   
    //    ▀█████████▄  ███    █▄     ▄████████    ▄████████    ▄████████    ▄████████ 
    //      ███    ███ ███    ███   ███    ███   ███    ███   ███    ███   ███    ███ 
    //      ███    ███ ███    ███   ███    █▀    ███    █▀    ███    █▀    ███    ███ 
    //     ▄███▄▄▄██▀  ███    ███  ▄███▄▄▄      ▄███▄▄▄      ▄███▄▄▄      ▄███▄▄▄▄██▀ 
    //    ▀▀███▀▀▀██▄  ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀     ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   
    //      ███    ██▄ ███    ███   ███          ███          ███    █▄  ▀███████████ 
    //      ███    ███ ███    ███   ███          ███          ███    ███   ███    ███ 
    //    ▄█████████▀  ████████▀    ███          ███          ██████████   ███    ███ 
    //                                                                     ███    ███ 
    if (m_renderConfiguration.postProcessing) {
        auto containerTexture = ResourceManager::getInstance().getTexture("container_texture");
        auto fboM = MeshGenerator::createScreenQuad(2.0f, 2.0f, containerTexture);
        m_framebufferMesh = std::make_unique<Mesh>(std::move(fboM));
        m_screenFbo = std::make_unique<FrameBuffer>(containerTexture);
    }

    //     ▄████████ ███    █▄  ▀█████████▄     ▄████████   ▄▄▄▄███▄▄▄▄      ▄████████    ▄███████▄ 
    //    ███    ███ ███    ███   ███    ███   ███    ███ ▄██▀▀▀███▀▀▀██▄   ███    ███   ███    ███ 
    //    ███    █▀  ███    ███   ███    ███   ███    █▀  ███   ███   ███   ███    ███   ███    ███ 
    //    ███        ███    ███  ▄███▄▄▄██▀   ▄███▄▄▄     ███   ███   ███   ███    ███   ███    ███ 
    //    ███        ███    ███ ▀▀███▀▀▀██▄  ▀▀███▀▀▀     ███   ███   ███ ▀███████████ ▀█████████▀  
    //    ███    █▄  ███    ███   ███    ██▄   ███    █▄  ███   ███   ███   ███    ███   ███        
    //    ███    ███ ███    ███   ███    ███   ███    ███ ███   ███   ███   ███    ███   ███        
    //    ████████▀  ████████▀  ▄█████████▀    ██████████  ▀█   ███   █▀    ███    █▀   ▄████▀      
    auto skyboxTexture = ResourceManager::getInstance().getTexture("skybox_texture");
    auto skyboxM = MeshGenerator::createAABBCuboid(1.0f, 1.0f, 1.0f, skyboxTexture);
    m_skyboxMesh = std::make_unique<Mesh>(std::move(skyboxM));
}

void OpenGLRenderer::beginFrame(int x, int y, int width, int height) {
    // Viewport
    glViewport(x, y, width, height);

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

void OpenGLRenderer::render(RenderState state) {
    if (m_renderConfiguration.postProcessing) {
        switch (state) {
            case RenderState::BEFORE:
                m_screenFbo->bind();

                glEnable(GL_DEPTH_TEST);
                glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            case RenderState::AFTER:
                m_screenFbo->unbind();

                glDisable(GL_DEPTH_TEST);
                //glDisable(GL_CULL_FACE);
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                m_screenTexShader->use();
                m_framebufferMesh->draw(*m_screenTexShader);
                break;
        }
    }
}

void OpenGLRenderer::stencilPass() {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}

void OpenGLRenderer::outlinePass(RenderState state) {
    switch (state) {
        case RenderState::BEFORE:
            glDisable(GL_DEPTH_TEST);
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
            glStencilMask(0x00);
            break;
        case RenderState::AFTER:
            glEnable(GL_DEPTH_TEST);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            glStencilMask(0xFF);
            glDisable(GL_STENCIL_TEST);
            break;
    }
}

void OpenGLRenderer::blendingPass(RenderState state) {
    switch (state) {
        case RenderState::BEFORE:
            glDisable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            // src: factor == source color vector, dst: factor == 1 - source color vector
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
            break;
        case RenderState::AFTER:
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            break;
    }
}

void OpenGLRenderer::topLayerPass() {
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::endFrame() {
    // no need to unbind it every time but w/e
    glBindVertexArray(0);

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_STENCIL_TEST);
    //glDisable(GL_BLEND);

    assert(m_windowRef);
    glfwSwapBuffers(m_windowRef);
}

void OpenGLRenderer::endFrameMinimap() {
    glDisable(GL_SCISSOR_TEST);
}

void OpenGLRenderer::renderImmediate(RenderState state) {
    switch (state) {
        case RenderState::BEFORE:
            glDisable(GL_DEPTH_TEST);

            m_renderImmediateShader->use();
            break;
        case RenderState::THIS:
            m_aabbMesh->draw(*m_renderImmediateShader);
            break;
        case RenderState::AFTER:
            glEnable(GL_DEPTH_TEST);
            break;
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