#include "../../configs/gl_config.hpp"
#include "../../configs/log_config.hpp"
#include "../../configs/math_config.hpp"
#include "../../ecs/registry.h"
#include "../../ecs/systems/ai_system.h"
#include "../../ecs/systems/camera_system.h"
#include "../../ecs/systems/dir_light_movement_system.h"
#include "../../ecs/systems/physics_system.h"
#include "../../ecs/systems/player_system.h"
#include "../../ecs/systems/render_system.h"
#include "../../ecs/systems/transform_system.h"
#include "../../managers/input_manager.h"
#include "../../managers/resource_manager.h"
#include "../../managers/scene_manager.h"
#include "../../utils/stats_constants.hpp"
#include "../backend.h"
#include "opengl_backend.h"
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

bool OpenGLBackEnd::init(unsigned int width, unsigned int height) {
    //        ███        ▄█    █▄     ▄█     ▄████████       ▄█     ▄████████       ▄█      ███    
    //    ▀█████████▄   ███    ███   ███    ███    ███      ███    ███    ███      ███  ▀█████████▄ 
    //       ▀███▀▀██   ███    ███   ███▌   ███    █▀       ███▌   ███    █▀       ███▌    ▀███▀▀██ 
    //        ███   ▀  ▄███▄▄▄▄███▄▄ ███▌   ███             ███▌   ███             ███▌     ███   ▀ 
    //        ███     ▀▀███▀▀▀▀███▀  ███▌ ▀███████████      ███▌ ▀███████████      ███▌     ███     
    //        ███       ███    ███   ███           ███      ███           ███      ███      ███     
    //        ███       ███    ███   ███     ▄█    ███      ███     ▄█    ███      ███      ███     
    //       ▄████▀     ███    █▀    █▀    ▄████████▀       █▀    ▄████████▀       █▀      ▄████▀   
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, "(C) Engine Runner 2049", nullptr, nullptr);
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
        LOG_E("BACKEND::GLFW_WINDOW_FAILED");
        glfwTerminate();
        return false;
    }

    if (glfwInit() == GL_FALSE) {
        LOG_E("BACKEND::GLFW_INIT_FAILED");
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
        LOG_E("BACKEND::GLEW_INIT_FAILED");
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return false;
    }

    // ONLY ONCE set 'this' as BackEnd
    glfwSetWindowUserPointer(m_window, this);

    //       ▄████████  ▄████████    ▄████████ 
    //      ███    ███ ███    ███   ███    ███ 
    //      ███    █▀  ███    █▀    ███    █▀  
    //     ▄███▄▄▄     ███          ███        
    //    ▀▀███▀▀▀     ███        ▀███████████ 
    //      ███    █▄  ███    █▄           ███ 
    //      ███    ███ ███    ███    ▄█    ███ 
    //      ██████████ ████████▀   ▄████████▀  
    m_registry.init();
    m_physicsSystem.init();
    m_renderSystem.init();
    //       ▄▄▄▄███▄▄▄▄      ▄████████ ███▄▄▄▄      ▄████████    ▄██████▄     ▄████████    ▄████████    ▄████████ 
    //     ▄██▀▀▀███▀▀▀██▄   ███    ███ ███▀▀▀██▄   ███    ███   ███    ███   ███    ███   ███    ███   ███    ███ 
    //     ███   ███   ███   ███    ███ ███   ███   ███    ███   ███    █▀    ███    █▀    ███    ███   ███    █▀  
    //     ███   ███   ███   ███    ███ ███   ███   ███    ███  ▄███         ▄███▄▄▄      ▄███▄▄▄▄██▀   ███        
    //     ███   ███   ███ ▀███████████ ███   ███ ▀███████████ ▀▀███ ████▄  ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   ▀███████████ 
    //     ███   ███   ███   ███    ███ ███   ███   ███    ███   ███    ███   ███    █▄  ▀███████████          ███ 
    //     ███   ███   ███   ███    ███ ███   ███   ███    ███   ███    ███   ███    ███   ███    ███    ▄█    ███ 
    //      ▀█   ███   █▀    ███    █▀   ▀█   █▀    ███    █▀    ████████▀    ██████████   ███    ███  ▄████████▀  
    //                                                                                     ███    ███              
    InputManager::getInstance().init();
    SceneManager::getInstance().init(m_registry);

    // set callbacks: window resize, single key click, mouse, scroll
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSetKeyCallback(m_window, key_callback);
    glfwSetCursorPosCallback(m_window, mouse_callback);
    glfwSetScrollCallback(m_window, scroll_callback);

    // hide mouse pointer
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    return true;
}

void OpenGLBackEnd::run() {
    // setup camera aspect ratio with "real" dimensions
    int width{};
    int height{};
    glfwGetFramebufferSize(m_window, &width, &height);
    m_cameraSystem.updateAspect(m_registry, width, height);
    // save time for FPS
    m_fpsLastTime = glfwGetTime();

    //       ▄▄▄▄███▄▄▄▄      ▄████████  ▄█  ███▄▄▄▄             
    //     ▄██▀▀▀███▀▀▀██▄   ███    ███ ███  ███▀▀▀██▄           
    //     ███   ███   ███   ███    ███ ███▌ ███   ███           
    //     ███   ███   ███   ███    ███ ███▌ ███   ███           
    //     ███   ███   ███ ▀███████████ ███▌ ███   ███           
    //     ███   ███   ███   ███    ███ ███  ███   ███           
    //     ███   ███   ███   ███    ███ ███  ███   ███           
    //      ▀█   ███   █▀    ███    █▀  █▀    ▀█   █▀            
    //                                                           
    //       ▄██████▄     ▄████████   ▄▄▄▄███▄▄▄▄      ▄████████ 
    //      ███    ███   ███    ███ ▄██▀▀▀███▀▀▀██▄   ███    ███ 
    //      ███    █▀    ███    ███ ███   ███   ███   ███    █▀  
    //     ▄███          ███    ███ ███   ███   ███  ▄███▄▄▄     
    //    ▀▀███ ████▄  ▀███████████ ███   ███   ███ ▀▀███▀▀▀     
    //      ███    ███   ███    ███ ███   ███   ███   ███    █▄  
    //      ███    ███   ███    ███ ███   ███   ███   ███    ███ 
    //      ████████▀    ███    █▀   ▀█   ███   █▀    ██████████ 
    //                                                           
    //     ▄█        ▄██████▄   ▄██████▄     ▄███████▄           
    //    ███       ███    ███ ███    ███   ███    ███           
    //    ███       ███    ███ ███    ███   ███    ███           
    //    ███       ███    ███ ███    ███   ███    ███           
    //    ███       ███    ███ ███    ███ ▀█████████▀            
    //    ███       ███    ███ ███    ███   ███                  
    //    ███▌    ▄ ███    ███ ███    ███   ███                  
    //    █████▄▄██  ▀██████▀   ▀██████▀   ▄████▀                
    //    ▀                                                      
    while (!glfwWindowShouldClose(m_window)) {
        //        ███      ▄█    ▄▄▄▄███▄▄▄▄      ▄████████ 
        //    ▀█████████▄ ███  ▄██▀▀▀███▀▀▀██▄   ███    ███ 
        //       ▀███▀▀██ ███▌ ███   ███   ███   ███    █▀  
        //        ███   ▀ ███▌ ███   ███   ███  ▄███▄▄▄     
        //        ███     ███▌ ███   ███   ███ ▀▀███▀▀▀     
        //        ███     ███  ███   ███   ███   ███    █▄  
        //        ███     ███  ███   ███   ███   ███    ███ 
        //       ▄████▀   █▀    ▀█   ███   █▀    ██████████ 
        double currentTime = glfwGetTime();
        double dt = currentTime - m_lastTime;
        // death spiral safe
        if (dt > 0.25) {
            dt = 0.25;
        }
        m_lastTime = currentTime;
        m_accumulator += dt;

        //       ▄████████  ▄█    █▄     ▄████████ ███▄▄▄▄       ███        ▄████████ 
        //      ███    ███ ███    ███   ███    ███ ███▀▀▀██▄ ▀█████████▄   ███    ███ 
        //      ███    █▀  ███    ███   ███    █▀  ███   ███    ▀███▀▀██   ███    █▀  
        //     ▄███▄▄▄     ███    ███  ▄███▄▄▄     ███   ███     ███   ▀   ███        
        //    ▀▀███▀▀▀     ███    ███ ▀▀███▀▀▀     ███   ███     ███     ▀███████████ 
        //      ███    █▄  ███    ███   ███    █▄  ███   ███     ███              ███ 
        //      ███    ███ ███    ███   ███    ███ ███   ███     ███        ▄█    ███ 
        //      ██████████  ▀██████▀    ██████████  ▀█   █▀     ▄████▀    ▄████████▀  
        InputManager::getInstance().copyKeys();
        // events to call InputManager
        glfwPollEvents();
        _processGlobalInput();
        // player's movement direction
        m_playerSystem.processInput(m_registry);
        // mouse scroll and movement
        m_cameraSystem.processInput(m_registry);

        // once per 60 frames
        while (m_accumulator >= Constants::Stats::World::FIXED_DT) {
            //     ▄█        ▄██████▄     ▄██████▄   ▄█   ▄████████ 
            //    ███       ███    ███   ███    ███ ███  ███    ███ 
            //    ███       ███    ███   ███    █▀  ███▌ ███    █▀  
            //    ███       ███    ███  ▄███        ███▌ ███        
            //    ███       ███    ███ ▀▀███ ████▄  ███▌ ███        
            //    ███       ███    ███   ███    ███ ███  ███    █▄  
            //    ███▌    ▄ ███    ███   ███    ███ ███  ███    ███ 
            //    █████▄▄██  ▀██████▀    ████████▀  █▀   ████████▀  
            //    ▀                                                 
            float fixedt = static_cast<float>(Constants::Stats::World::FIXED_DT);
            // interpolation setup
            m_transformSystem.saveState(m_registry);
            // actual movement
            m_aiSystem.fixedUpdate(m_registry, fixedt);
            m_dirLightMovementSystem.fixedUpdate(m_registry, fixedt);
            m_playerSystem.fixedUpdate(m_registry, fixedt);
            // physics step: collisions
            m_physicsSystem.fixedUpdate(m_registry, fixedt);
            m_physicsSystem.execute();
            m_accumulator -= Constants::Stats::World::FIXED_DT;
        }

        //       ▄████████    ▄████████ ███▄▄▄▄   ████████▄     ▄████████    ▄████████ 
        //      ███    ███   ███    ███ ███▀▀▀██▄ ███   ▀███   ███    ███   ███    ███ 
        //      ███    ███   ███    █▀  ███   ███ ███    ███   ███    █▀    ███    ███ 
        //     ▄███▄▄▄▄██▀  ▄███▄▄▄     ███   ███ ███    ███  ▄███▄▄▄      ▄███▄▄▄▄██▀ 
        //    ▀▀███▀▀▀▀▀   ▀▀███▀▀▀     ███   ███ ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀   
        //    ▀███████████   ███    █▄  ███   ███ ███    ███   ███    █▄  ▀███████████ 
        //      ███    ███   ███    ███ ███   ███ ███   ▄███   ███    ███   ███    ███ 
        //      ███    ███   ██████████  ▀█   █▀  ████████▀    ██████████   ███    ███ 
        //      ███    ███                                                  ███    ███ 
        // interpolation (smoothing the frames in-between physics and rendering)
        float alpha = static_cast<float>(m_accumulator / Constants::Stats::World::FIXED_DT);
        m_renderSystem.beginFrame(width, height);
        // calculate render context and register queues
        m_renderSystem.update(m_registry, alpha);
        // sort and render queues
        m_renderSystem.execute();
        // debug mode
        m_renderSystem.renderImmediate();
        m_renderSystem.endFrame(m_window);

        // reset input changes for mouse scroll and movement
        InputManager::getInstance().reset();
        // bonus
        _showFps(m_window, currentTime);
    }

    //       ▄██████▄     ▄████████   ▄▄▄▄███▄▄▄▄      ▄████████ 
    //      ███    ███   ███    ███ ▄██▀▀▀███▀▀▀██▄   ███    ███ 
    //      ███    █▀    ███    ███ ███   ███   ███   ███    █▀  
    //     ▄███          ███    ███ ███   ███   ███  ▄███▄▄▄     
    //    ▀▀███ ████▄  ▀███████████ ███   ███   ███ ▀▀███▀▀▀     
    //      ███    ███   ███    ███ ███   ███   ███   ███    █▄  
    //      ███    ███   ███    ███ ███   ███   ███   ███    ███ 
    //      ████████▀    ███    █▀   ▀█   ███   █▀    ██████████ 
    //                                                           
    //     ▄██████▄   ▄█    █▄     ▄████████    ▄████████        
    //    ███    ███ ███    ███   ███    ███   ███    ███        
    //    ███    ███ ███    ███   ███    █▀    ███    ███        
    //    ███    ███ ███    ███  ▄███▄▄▄      ▄███▄▄▄▄██▀        
    //    ███    ███ ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀▀▀          
    //    ███    ███ ███    ███   ███    █▄  ▀███████████        
    //    ███    ███ ███    ███   ███    ███   ███    ███        
    //     ▀██████▀   ▀██████▀    ██████████   ███    ███        
    //                                         ███    ███        
    // delete raw pointers inside containers
    // no raw pointers for now so chill

    glfwDestroyWindow(m_window);
    m_window = nullptr;
    glfwTerminate();
}

void OpenGLBackEnd::setViewport(int x, int y, int width, int height) {
    glViewport(x, y, width, height);
}

void OpenGLBackEnd::_processGlobalInput() {
    // EXIT
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window, true);
    }

    //    ████████▄     ▄████████ ▀█████████▄  ███    █▄     ▄██████▄  
    //    ███   ▀███   ███    ███   ███    ███ ███    ███   ███    ███ 
    //    ███    ███   ███    █▀    ███    ███ ███    ███   ███    █▀  
    //    ███    ███  ▄███▄▄▄      ▄███▄▄▄██▀  ███    ███  ▄███        
    //    ███    ███ ▀▀███▀▀▀     ▀▀███▀▀▀██▄  ███    ███ ▀▀███ ████▄  
    //    ███    ███   ███    █▄    ███    ██▄ ███    ███   ███    ███ 
    //    ███   ▄███   ███    ███   ███    ███ ███    ███   ███    ███ 
    //    ████████▀    ██████████ ▄█████████▀  ████████▀    ████████▀  
#ifdef _DEBUG
// HOTLOAD SHADERS
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_L)) {
        ResourceManager::getInstance().reloadShaders();
    }
    // SCENES
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_K)) {
        SceneManager::getInstance().toggleScene(m_registry);
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
        m_cameraSystem.logPosition(m_registry);
    }
#endif
}

void OpenGLBackEnd::_showFps(GLFWwindow* window, double currentTime) {
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