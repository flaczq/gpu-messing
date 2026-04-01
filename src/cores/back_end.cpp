#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "../graphics/renderer.h"
#include "../managers/input_manager.h"
#include "../managers/resource_manager.h"
#include "../managers/scene_manager.h"
#include "../physics/physics_world.h"
#include "back_end.h"
#include "camera.h"
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

BackEnd::BackEnd(GraphicsAPI graphicsAPI, unsigned int width, unsigned int height)
    : m_screenWidth(width),
      m_screenHeight(height)
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

    // configuration: experimental GgLEW + Z-depth test
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);

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
    m_camera = std::make_unique<Camera>(m_window, m_screenWidth, m_screenHeight);
    m_camera->init();
    m_renderer = std::make_unique<Renderer>(m_window);
    m_renderer->init();
    m_sceneManager = std::make_unique<SceneManager>(m_camera.get());
    m_sceneManager->init();
    m_physicsWorld = std::make_unique<PhysicsWorld>();
    m_physicsWorld->init();

    //    ┳┓┏┓┏┓┏┓┳┳┳┓┏┓┏┓┏┓
    //    ┣┫┣ ┗┓┃┃┃┃┣┫┃ ┣ ┗┓
    //    ┛┗┗┛┗┛┗┛┗┛┛┗┗┛┗┛┗┛
    //                      
    ResourceManager::getInstance().loadShader("model_shader", "../shaders/model.vert", "../shaders/model.frag");
    ResourceManager::getInstance().loadShader("light_shader", "../shaders/light.vert", "../shaders/light.frag");

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓  ┏┓┳┓•┳┳┓┳┏┳┓•┓┏┏┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣   ┃┃┣┫┓┃┃┃┃ ┃ ┓┃┃┣ ┗┓
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛  ┣┛┛┗┗┛ ┗┻ ┻ ┗┗┛┗┛┗┛
    //                                           
    //diffuseMapTP = TexturePrimitive::load("../assets/container2.png");
    //specularMapTP = TexturePrimitive::load("../assets/container2_specular.png");

    // set callbacks: single key click, mouse, scroll
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

        // events to call InputManager
        glfwPollEvents();

        // game logic: physics, movement, ai, collisions
        processCommonInput();
        m_camera->processInput(dt);

        // 1 per 60 frames
        while (m_accumulator >= FIXED_DT) {
            //m_physicsWorld->saveState();
            //m_physicsWorld->fixedUpdate(static_cast<float>(FIXED_DT));
            m_sceneManager->saveState();
            m_sceneManager->fixedUpdate(static_cast<float>(FIXED_DT));
            m_accumulator -= FIXED_DT;
        }

        // Interpolation (smoothing the frames in-between physics and rendering)
        float alpha = static_cast<float>(m_accumulator / FIXED_DT);
        // renderrring at last
        m_renderer->beginFrame();
        m_sceneManager->renderFrame(alpha);
        m_renderer->endFrame();

        //TexturePrimitive::bind(diffuseMapTP, 0);
        //TexturePrimitive::bind(specularMapTP, 1);

        // reset input changes
        InputManager::getInstance().reset();

        // bonus
        showFps(m_window, currentTime);
    }

    //    ┏┓┏┓┳┳┓┏┓  ┏┓┓┏┏┓┳┓
    //    ┃┓┣┫┃┃┃┣   ┃┃┃┃┣ ┣┫
    //    ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┛┗
    //                       
    ResourceManager::getInstance().clear();

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void BackEnd::processCommonInput() {
    auto& input = InputManager::getInstance();

    // EXIT
    if (input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window, true);
    }

    //    ┳┓┏┓┳┓┳┳┏┓
    //    ┃┃┣ ┣┫┃┃┃┓
    //    ┻┛┗┛┻┛┗┛┗┛
    //              
    #ifdef _DEBUG
    // GOD MODE
    if (input.isKeyPressed(GLFW_KEY_G)) {
        m_camera->toggleGodMode();
    }

    // SCENES
    if (input.isKeyPressed(GLFW_KEY_P)) {
        m_sceneManager->toggleScene();
    }

    // RENDER MODE
    if (input.isKeyPressed(GLFW_KEY_O)) {
        m_renderer->toggleRenderMode();
    }

    // INFO: POSITION, CAMERA
    if (input.isKeyPressed(GLFW_KEY_I)) {
        glm::mat4 view = m_camera->getViewMatrix(0.0f);
        displayPosition(view);
        displayCameraAngles(view);
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

void BackEnd::displayPosition(glm::mat4 viewMatrix) {
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec3 pos = glm::vec3(inverseView[3]);

    std::cout << std::fixed << std::setprecision(2);
    LOG_D("Position: "
        << "X: " << std::showpos << pos.x << "   "
        << "Y: " << std::showpos << pos.y << "   "
        << "Z: " << std::showpos << pos.z);
}

void BackEnd::displayCameraAngles(glm::mat4 viewMatrix) {
    glm::vec3 scale;
    glm::quat orientation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    if (glm::decompose(viewMatrix, scale, orientation, translation, skew, perspective)) {
        glm::vec3 euler = glm::eulerAngles(orientation);

        float pitch = glm::degrees(euler.x);
        float yaw = glm::degrees(euler.y);
        float roll = glm::degrees(euler.z);

        std::cout << std::fixed << std::setprecision(2);
        LOG_D("Camera: "
            << "Pitch: " << std::showpos << pitch << " deg   "
            << "Yaw: " << std::showpos << yaw << " deg   "
            << "Roll: " << std::showpos << roll << " deg");
    }
}