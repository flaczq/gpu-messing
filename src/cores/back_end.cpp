#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../game/camera.h"
#include "../game/physics_world.h"
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

    //    •┳┓•┏┳┓
    //    ┓┃┃┓ ┃ 
    //    ┗┛┗┗ ┻ 
    //           
    m_camera = std::make_unique<Camera>(m_screenWidth, m_screenHeight);
    m_camera->init();
    // top-view minimap
    if (m_minimap) {
        m_minimapCamera = std::make_unique<Camera>(m_minimapWidth, m_minimapHeight);
        m_minimapCamera->setViewPos(glm::vec3(8.0f, 20.0f, 15.0f));
        m_minimapCamera->setYaw(-90.0f);
        m_minimapCamera->setPitch(-70.0f);
        m_minimapCamera->init();
    }
    SceneManager::getInstance().init(m_camera.get());
    Renderer::getInstance().init(m_window, m_camera.get());
    //m_physicsWorld = std::make_unique<PhysicsWorld>();
    //m_physicsWorld->init();

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
        m_camera->processInput();
        m_camera->updateVectors();
        if (m_minimap) {
            //m_minimapCamera->processInput();
            //m_minimapCamera->updateVectors();
        }

        // 1 per 60 frames
        while (m_accumulator >= FIXED_DT) {
            //m_physicsWorld->saveState();
            //m_physicsWorld->fixedUpdate(static_cast<float>(FIXED_DT));
            SceneManager::getInstance().saveState();
            m_camera->saveState();
            SceneManager::getInstance().fixedUpdate(static_cast<float>(FIXED_DT));
            // actual movement
            m_camera->fixedUpdate(static_cast<float>(FIXED_DT));
            if (m_minimap) {
                //m_minimapCamera->saveState();
                //m_minimapCamera->fixedUpdate(static_cast<float>(FIXED_DT));
            }
            m_accumulator -= FIXED_DT;
        }

        // Interpolation (smoothing the frames in-between physics and rendering)
        float alpha = static_cast<float>(m_accumulator / FIXED_DT);

        // lookAt()
        m_camera->updateView(alpha);
        m_camera->updateProjection();
        if (m_minimap) {
            m_minimapCamera->updateView(alpha);
            //m_minimapCamera->updateProjection();
        }

        // renderrring at last
        // --- main camera
        Renderer::getInstance().beginFrame(m_screenWidth, m_screenHeight);
        SceneManager::getInstance().update(alpha);
        // --- minimap camera
        if (m_minimap) {
            Renderer::getInstance().setCamera(m_minimapCamera.get());
            Renderer::getInstance().beginFrameMinimap(m_minimapWidth, m_minimapHeight);
            SceneManager::getInstance().update(alpha);
            Renderer::getInstance().endFrameMinimap();
            Renderer::getInstance().setCamera(m_camera.get());
        }
        Renderer::getInstance().endFrame();

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
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_P)) {
        SceneManager::getInstance().toggleScene();
    }
    // RENDER MODE
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_O)) {
        Renderer::getInstance().toggleRenderMode();
    }
    // INFO: POSITION, CAMERA
    if (InputManager::getInstance().isKeyPressed(GLFW_KEY_I)) {
        displayCameraData();
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

void BackEnd::displayCameraData() {
    glm::vec3 cameraPos = m_camera->getViewPos();
    std::cout << std::fixed << std::setprecision(2);
    LOG_D("Camera: "
        << "X: " << std::showpos << cameraPos.x << "   "
        << "Y: " << std::showpos << cameraPos.y << "   "
        << "Z: " << std::showpos << cameraPos.z << "   "
        << "YAW: " << m_camera->getYaw() << "   "
        << "PITCH: " << m_camera->getPitch());
}