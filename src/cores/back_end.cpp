#include "back_end.h"
#include "camera.h"
#include "renderer.h"
#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../managers/scene_manager.h"
#include <memory>
#include <iostream>
#include <string>
#include <thread>
#include <iomanip>
#include <ios>

BackEnd::BackEnd(GraphicsAPI graphicsAPI, unsigned int width, unsigned int height) {
    if (graphicsAPI == GraphicsAPI::OPEN_GL) {
        std::cout << "*** OpenGL for Windows" << std::endl << std::endl;
    } else if (graphicsAPI == GraphicsAPI::VULKAN) {
        std::cout << "*** Vulkan for Windows" << std::endl << std::endl;
        throw std::logic_error("Not implemented for Vulkan... yet");
    }

    m_screenWidth = width;
    m_screenHeight = height;
}

BackEnd::~BackEnd() {
    // simple data structures (not meshes)

    //TexturePrimitive::clean(diffuseMapTP);
    //TexturePrimitive::clean(specularMapTP);

    glfwTerminate();
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
    std::cout << R"(
     ## cells interlinked within cells ##
      ___   ___  _  _   ___  
     |__ \ / _ \| || | / _ \ 
        ) | | | | || || (_) |
       / /| | | |__   _\__, |
      / /_| |_| |  | |  / / 
     |____|\___/   |_| /_/  
    )" << std::endl;

    if (m_window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    if (glfwInit() == GL_FALSE) {
        std::cerr << "Failed to init GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }

    // sleep and make openGL window focused
    glfwMakeContextCurrent(m_window);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    glfwFocusWindow(m_window);
    glfwShowWindow(m_window);

    // configuration: experimental GgLEW + Z-depth test
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);

    if (glewInit()) {
        std::cerr << "Failed to init GLEW" << std::endl;
        glfwDestroyWindow(m_window);
        glfwTerminate();
        return -1;
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

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓  ┏┓┳┓•┳┳┓┳┏┳┓•┓┏┏┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣   ┃┃┣┫┓┃┃┃┃ ┃ ┓┃┃┣ ┗┓
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛  ┣┛┛┗┗┛ ┗┻ ┻ ┗┗┛┗┛┗┛
    //                                           
    //diffuseMapTP = TexturePrimitive::load("../assets/container2.png");
    //specularMapTP = TexturePrimitive::load("../assets/container2_specular.png");

    // set callbacks: single key click
    glfwSetKeyCallback(m_window, key_callback);

    return 1;
}

void BackEnd::run() {
    //    ┓┏┏┓┳┓•┏┓┳┓┓ ┏┓┏┓
    //    ┃┃┣┫┣┫┓┣┫┣┫┃ ┣ ┗┓
    //    ┗┛┛┗┛┗┗┛┗┻┛┗┛┗┛┗┛
    //                     
    glm::vec3 lightPos(1.0f, 2.0f, 3.0f);
    float radius = 2.0f;

    //    ┳┳┓┏┓•┳┓  ┏┓┏┓┳┳┓┏┓  ┓ ┏┓┏┓┏┓
    //    ┃┃┃┣┫┓┃┃  ┃┓┣┫┃┃┃┣   ┃ ┃┃┃┃┃┃
    //    ┛ ┗┛┗┗┛┗  ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┣┛
    //                                 
    while (!glfwWindowShouldClose(m_window)) {
        // values (deltaTime)
        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - m_lastFrame;
        m_lastFrame = currentTime;
        lightPos.x = cos(currentTime) * radius;
        lightPos.y = sin(currentTime) * radius;

        // bonus
        showFps(m_window, currentTime);

        // events
        glfwPollEvents();
        m_camera->processInput(dt);

        // physics, movement, ai, collisions
        m_sceneManager->update(dt);

        // rendering at last
        m_renderer->beginFrame();
        m_sceneManager->render();
        m_renderer->endFrame();

        //TexturePrimitive::bind(diffuseMapTP, 0);
        //TexturePrimitive::bind(specularMapTP, 1);
    }
}

Camera* BackEnd::getCamera() const {
    return m_camera.get();
}

Renderer* BackEnd::getRenderer() const {
    return m_renderer.get();
}

SceneManager* BackEnd::getSceneManager() const {
    return m_sceneManager.get();
}

void BackEnd::showFps(GLFWwindow* window, double currentTime) {
    m_nbFrames++;
    if (currentTime - m_lastTime >= 1.0) {
        double fps = double(m_nbFrames);
        double msPerFrame = 1000.0 / double(m_nbFrames);
        std::string title = "(C) Engine Runner 2049 - FPS: " + std::to_string((int)fps) + " (" + std::to_string(msPerFrame).substr(0, 4) + " ms)";
        glfwSetWindowTitle(window, title.c_str());
        m_nbFrames = 0;
        m_lastTime += 1.0;
    }
}

void BackEnd::displayPosition(glm::mat4 viewMatrix) {
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec3 pos = glm::vec3(inverseView[3]);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "* Position Status" << std::endl;
    std::cout << "X: " << std::showpos << pos.x << "   "
        << "Y: " << std::showpos << pos.y << "   "
        << "Z: " << std::showpos << pos.z << std::endl;
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
        std::cout << "* Camera Status" << std::endl;
        std::cout << "Pitch: " << std::showpos << pitch << " deg   "
            << "Yaw: " << std::showpos << yaw << " deg   "
            << "Roll: " << std::showpos << roll << " deg" << std::endl;
    }
}

// single key click
void BackEnd::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));

    // CROUCHING/STANDING
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        backEnd->getCamera()->toggleCameraMode();
    }

    // SPOTLIGHT
    /*if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        bool& spotlightOn = std::get<bool>(backEnd->uniformVars["spotlightOn"]);
        spotlightOn = !spotlightOn;
        std::cout << "* Changed spotlight to: " << std::boolalpha << spotlightOn << std::endl;
    }*/

    //    ┳┓┏┓┳┓┳┳┏┓
    //    ┃┃┣ ┣┫┃┃┃┓
    //    ┻┛┗┛┻┛┗┛┗┛
    //              
    #ifdef _DEBUG
    // GOD MODE
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        backEnd->getCamera()->toggleGodMode();
    }

    // SCENES
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        backEnd->getSceneManager()->toggleScene();
    }

    // RENDER MODE
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        backEnd->getRenderer()->toggleRenderMode();
    }

    // INFO: POSITION, CAMERA
    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        backEnd->displayPosition(backEnd->getCamera()->getViewMatrix());
        backEnd->displayCameraAngles(backEnd->getCamera()->getViewMatrix());
    }
    #endif
};