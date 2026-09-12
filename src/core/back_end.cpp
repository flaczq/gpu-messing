#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../ecs/registry.h"
#include "../ecs/systems/ai_system.h"
#include "../ecs/systems/camera_system.h"
#include "../ecs/systems/physics_system.h"
#include "../ecs/systems/player_system.h"
#include "../ecs/systems/render_system.h"
#include "../ecs/systems/transform_system.h"
#include "../game/camera.h"
#include "../graphics/renderer.h"
#include "../managers/input_manager.h"
#include "../managers/resource_manager.h"
#include "../managers/scene_manager.h"
#include "back_end.h"
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

BackEnd::BackEnd(GraphicsAPI graphicsAPI, unsigned int width, unsigned int height)
    : m_screenWidth(width),
      m_screenHeight(height),
      m_minimapWidth(width / 4),
      m_minimapHeight(height / 4)
{
    if (graphicsAPI == GraphicsAPI::OPEN_GL) {
        LOG("*** OpenGL for Windows");
    } else if (graphicsAPI == GraphicsAPI::VULKAN) {
        LOG_E("*** Vulkan for Windows - Not implemented... yet");
        throw std::logic_error("Not implemented for Vulkan... yet");
    }
}

bool BackEnd::init() {
    //    ┏┳┓┓┏•┏┓  •┏┓  •┏┳┓
    //     ┃ ┣┫┓┗┓  ┓┗┓  ┓ ┃ 
    //     ┻ ┛┗┗┗┛  ┗┗┛  ┗ ┻ 
    //                       
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(m_screenWidth, m_screenHeight, "(C) Engine Runner 2049", nullptr, nullptr);
    LOG(R"(
     ## cells interlinked within cells ##
      ___   ___  _  _   ___  
     |__ \ / _ \| || | / _ \ 
        ) | | | | || || (_) |
       / /| | | |__   _\__, |
      / /_| |_| |  | |  / / 
     |____|\___/   |_| /_/  
    )");

    if (m_window == nullptr) {
        LOG_E("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    if (glfwInit() == GL_FALSE) {
        LOG_E("Failed to init GLFW");
        glfwTerminate();
        return false;
    }

    // sleep and make OpenGL window focused
    glfwMakeContextCurrent(m_window);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    glfwFocusWindow(m_window);
    glfwShowWindow(m_window);

    // configuration: experimental GgLEW
    glewExperimental = GL_TRUE;

    if (glewInit()) {
        LOG_E("Failed to init GLEW");
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }

    // ONLY ONCE set 'this' as BackEnd
    glfwSetWindowUserPointer(m_window, this);

    //    __/\\\\\\\\\\\\\\\________/\\\\\\\\\_____/\\\\\\\\\\\___        
    //     _\/\\\///////////______/\\\////////____/\\\/////////\\\_       
    //      _\/\\\_______________/\\\/____________\//\\\______\///__      
    //       _\/\\\\\\\\\\\______/\\\_______________\////\\\_________     
    //        _\/\\\///////______\/\\\__________________\////\\\______    
    //         _\/\\\_____________\//\\\____________________\////\\\___   
    //          _\/\\\______________\///\\\___________/\\\______\//\\\__  
    //           _\/\\\\\\\\\\\\\\\____\////\\\\\\\\\_\///\\\\\\\\\\\/___ 
    //            _\///////////////________\/////////____\///////////_____
    m_registry.init();
    //m_transformSystem.init();
    m_physicsSystem.init();
    //m_playerSystem.init();
    //m_aiSystem.init();
    //m_cameraSystem.init();
    m_renderSystem.init(m_window);
    //    __/\\\\\\\\\\\\\\\________/\\\\\\\\\_____/\\\\\\\\\\\___        
    //     _\/\\\///////////______/\\\////////____/\\\/////////\\\_       
    //      _\/\\\_______________/\\\/____________\//\\\______\///__      
    //       _\/\\\\\\\\\\\______/\\\_______________\////\\\_________     
    //        _\/\\\///////______\/\\\__________________\////\\\______    
    //         _\/\\\_____________\//\\\____________________\////\\\___   
    //          _\/\\\______________\///\\\___________/\\\______\//\\\__  
    //           _\/\\\\\\\\\\\\\\\____\////\\\\\\\\\_\///\\\\\\\\\\\/___ 
    //            _\///////////////________\/////////____\///////////_____

    //    •┳┓•┏┳┓
    //    ┓┃┃┓ ┃ 
    //    ┗┛┗┗ ┻ 
    //           
    //m_camera = std::make_unique<Camera>(m_screenWidth, m_screenHeight);
    //m_camera.init();
    //// top-view minimap
    //if (m_minimap) {
    //    m_minimapCamera = std::make_unique<Camera>(m_minimapWidth, m_minimapHeight);
    //    //m_minimapCamera->setViewPos(glm::vec3(8.0f, 20.0f, 15.0f));
    //    //m_minimapCamera->setPreViewPos(glm::vec3(8.0f, 20.0f, 15.0f));
    //    //m_minimapCamera->setYaw(-90.0f);
    //    //m_minimapCamera->setPitch(-70.0f);
    //    // FIXME: follow empty transform
    //    m_minimapCamera->init();
    //}
    // nothing else matters... but order
    SceneManager::getInstance().init(m_registry, m_physicsSystem, m_cameraSystem);

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓  ┏┓┳┓•┳┳┓┳┏┳┓•┓┏┏┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣   ┃┃┣┫┓┃┃┃┃ ┃ ┓┃┃┣ ┗┓
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛  ┣┛┛┗┗┛ ┗┻ ┻ ┗┗┛┗┛┗┛
    //                                           
    //diffuseMapTP = TexturePrimitive::load("../assets/container2.png");
    //specularMapTP = TexturePrimitive::load("../assets/container2_specular.png");

    // set callbacks: window resize, single key click, mouse, scroll
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);

    // hide mouse pointer
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void BackEnd::run() {
    m_fpsLastTime = glfwGetTime();

    //    ┳┳┓┏┓•┳┓   ┓ ┏┓┏┓┏┓
    //    ┃┃┃┣┫┓┃┃   ┃ ┃┃┃┃┃┃
    //    ┛ ┗┛┗┗┛┗   ┗┛┗┛┗┛┣┛
    //                                 
    // time -> events -> logic -> render
    while (!glfwWindowShouldClose(m_window)) {
        // deltaTime
        double currentTime = glfwGetTime();
        double dt = currentTime - m_lastTime;
        // death spiral safe
        if (dt > 0.25) {
            dt = 0.25;
        }
        m_lastTime = currentTime;
        m_accumulator += dt;

        InputManager::getInstance().copyKeys();
        // events to call InputManager
        glfwPollEvents();

        processGlobalInput();
        // player movement direction
        m_playerSystem.processInput(m_registry);
        // mouse scroll and movement
        m_cameraSystem.processInput(m_registry);
        //if (m_minimap) {
        //    m_minimapCamera->processInput();
        //}

        // logic (once per 60 frames): physics, collisions
        while (m_accumulator >= FIXED_DT) {
            float fixedt = static_cast<float>(FIXED_DT);
            m_transformSystem.saveState(m_registry);

            // change transform position based on set direction
            SceneManager::getInstance().fixedUpdate(fixedt);

            // execute physics command from queue with collisions
            m_physicsSystem.execute();
            m_accumulator -= FIXED_DT;
        }

        // Interpolation (smoothing the frames in-between physics and rendering)
        float alpha = static_cast<float>(m_accumulator / FIXED_DT);
        // lookAt()
        m_cameraSystem.updateView(m_registry, alpha);
        // if it's only used in RenderSystem - just calculate it there and delete this
        m_cameraSystem.updateProjection(m_registry);
        //if (m_minimap) {
        //    m_minimapCamera->updateView(alpha);
        //    m_minimapCamera->updateProjection();
        //}

        // renderrring at last
        // --- main camera
        m_renderSystem.beginFrame(m_screenWidth, m_screenHeight);
        // Renderer -> registerInQueue()
        SceneManager::getInstance().update(alpha);
        // execute drawing commands from queues
        m_renderSystem.execute();
        // probably for debug only
        m_renderSystem.renderImmediate();
        // --- minimap camera
        if (m_minimap) {
            //m_renderSystem.setCamera(m_minimapCamera.get());
            m_renderSystem.beginFrameMinimap(m_minimapWidth, m_minimapHeight);
            SceneManager::getInstance().update(alpha);
            m_renderSystem.execute();
            m_renderSystem.endFrameMinimap();
            //m_renderSystem.setCamera(m_camera.get());
        }
        m_renderSystem.endFrame();

        //TexturePrimitive::bind(diffuseMapTP, 0);
        //TexturePrimitive::bind(specularMapTP, 1);

        // game entities clean up
        SceneManager::getInstance().lateUpdate();
        // reset input changes for mouse
        InputManager::getInstance().reset();

        // bonus
        showFps(m_window, currentTime);
    }

    //    ┏┓┏┓┳┳┓┏┓  ┏┓┓┏┏┓┳┓
    //    ┃┓┣┫┃┃┃┣   ┃┃┃┃┣ ┣┫
    //    ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┛┗
    //                       
    SceneManager::getInstance().end();
    ResourceManager::getInstance().end();
    m_registry.end();

    glfwDestroyWindow(m_window);
    m_window = nullptr;
    glfwTerminate();
}

void BackEnd::processGlobalInput() {
    // EXIT
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window, true);
    }

    //    ┳┓┏┓┳┓┳┳┏┓
    //    ┃┃┣ ┣┫┃┃┃┓
    //    ┻┛┗┛┻┛┗┛┗┛
    //              
    #ifdef _DEBUG
    // HOTLOAD SHADERS
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_L)) {
        ResourceManager::getInstance().reloadShaders();
    }
    // SCENES
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_K)) {
        SceneManager::getInstance().toggleScene();
    }
    // RENDER MODE
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_O)) {
        m_renderSystem.toggleRenderMode();
    }
    // RENDER DEBUG MODE
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_P)) {
        m_renderSystem.toggleRenderDebugMode();
    }
    // INFO: POSITION, CAMERA
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_I)) {
        m_cameraSystem.logMainCameraPosition(m_registry);
    }
    #endif
}

void BackEnd::showFps(GLFWwindow* window, double currentTime) {
    m_fpsNr++;
    // update every 1sec
    if (currentTime - m_fpsLastTime >= 1.0) {
        double fps = double(m_fpsNr);
        double msPerFrame = 1000.0 / double(m_fpsNr);
        std::string title = "(C) Engine Runner 2049 - FPS: " + std::to_string((int)fps) + " (" + std::to_string(msPerFrame).substr(0, 4) + " ms)";
        glfwSetWindowTitle(window, title.c_str());
        m_fpsNr = 0;
        m_fpsLastTime += 1.0;
    }
}