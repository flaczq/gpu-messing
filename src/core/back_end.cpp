#include "back_end.h"

BackEnd::BackEnd(GraphicsAPI graphicsAPI, unsigned int width, unsigned int height) {
    if (graphicsAPI == GraphicsAPI::OPEN_GL) {
        std::cout << "*** OpenGL for Windows" << std::endl << std::endl;
    } else if (graphicsAPI == GraphicsAPI::VULKAN) {
        std::cout << "*** Vulkan for Windows" << std::endl << std::endl;
        throw std::logic_error("Not implemented for Vulkan... yet");
    }

    screenWidth = width;
    screenHeight = height;
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

    window = glfwCreateWindow(screenWidth, screenHeight, "(C) Engine Runner 2049", nullptr, nullptr);
    std::cout << R"(
     ## cells interlinked within cells ##
      ___   ___  _  _   ___  
     |__ \ / _ \| || | / _ \ 
        ) | | | | || || (_) |
       / /| | | |__   _\__, |
      / /_| |_| |  | |  / / 
     |____|\___/   |_| /_/  
    )" << std::endl;

    if (window == nullptr) {
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
    glfwMakeContextCurrent(window);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    glfwFocusWindow(window);
    glfwShowWindow(window);

    // configuration: experimental GgLEW + Z-depth test
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);

    if (glewInit()) {
        std::cerr << "Failed to init GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // ONLY ONCE set 'this' as BackEnd
    glfwSetWindowUserPointer(window, this);

    //    •┳┓•┏┳┓
    //    ┓┃┃┓ ┃ 
    //    ┗┛┗┗ ┻ 
    //           
    camera = std::make_unique<Camera>(window, screenWidth, screenHeight);
    camera->init();
    renderer = std::make_unique<Renderer>(window);
    renderer->init();
    sceneManager = std::make_unique<SceneManager>(camera.get());
    sceneManager->init();

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓  ┏┓┳┓•┳┳┓┳┏┳┓•┓┏┏┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣   ┃┃┣┫┓┃┃┃┃ ┃ ┓┃┃┣ ┗┓
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛  ┣┛┛┗┗┛ ┗┻ ┻ ┗┗┛┗┛┗┛
    //                                           
    //diffuseMapTP = TexturePrimitive::load("../assets/container2.png");
    //specularMapTP = TexturePrimitive::load("../assets/container2_specular.png");

    // set callbacks: single key click
    glfwSetKeyCallback(window, key_callback);

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
    while (!glfwWindowShouldClose(window)) {
        // values (deltaTime)
        float currentTime = static_cast<float>(glfwGetTime());
        float dt = currentTime - lastFrame;
        lastFrame = currentTime;
        lightPos.x = cos(currentTime) * radius;
        lightPos.y = sin(currentTime) * radius;

        // bonus
        showFps(window, currentTime);

        // events
        glfwPollEvents();
        camera->processInput(dt);

        // physics, movement, ai, collisions
        sceneManager->update(dt);

        // rendering at last
        renderer->beginFrame();
        sceneManager->render();
        renderer->endFrame();

        //TexturePrimitive::bind(diffuseMapTP, 0);
        //TexturePrimitive::bind(specularMapTP, 1);
    }
}

void BackEnd::showFps(GLFWwindow* window, double currentTime) {
    nbFrames++;
    if (currentTime - lastTime >= 1.0) {
        double fps = double(nbFrames);
        double msPerFrame = 1000.0 / double(nbFrames);
        std::string title = "(C) Engine Runner 2049 - FPS: " + std::to_string((int)fps) + " (" + std::to_string(msPerFrame).substr(0, 4) + " ms)";
        glfwSetWindowTitle(window, title.c_str());
        nbFrames = 0;
        lastTime += 1.0;
    }
}

void BackEnd::displayPosition(glm::mat4 viewMatrix) {
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec3 pos = glm::vec3(inverseView[3]);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "* Position Status" << std::endl;
    std::cout << "X: " << std::showpos << pos.x << "   "
        << "Y: " << std::showpos << pos.y << "   "
        << "Z: " << std::showpos << pos.z << std::endl << std::endl;
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
            << "Roll: " << std::showpos << roll << " deg" << std::endl << std::endl;
    }
}

// single key click
void BackEnd::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    BackEnd* backEnd = static_cast<BackEnd*>(glfwGetWindowUserPointer(window));

    // CROUCHING/STANDING
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        backEnd->camera->toggleCameraMode();
    }

    // SPOTLIGHT
    /*if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        bool& spotlightOn = std::get<bool>(backEnd->uniformVars["spotlightOn"]);
        spotlightOn = !spotlightOn;
        std::cout << "* Changed spotlight to: " << std::boolalpha << spotlightOn << std::endl << std::endl;
    }*/

    //    ┳┓┏┓┳┓┳┳┏┓
    //    ┃┃┣ ┣┫┃┃┃┓
    //    ┻┛┗┛┻┛┗┛┗┛
    //              
    #ifdef _DEBUG
    // GOD MODE
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        backEnd->camera->toggleGodMode();
    }

    // SCENES
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        backEnd->sceneManager->toggleScene();
    }

    // RENDER MODE
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        backEnd->renderer->toggleRenderMode();
    }

    // INFO: POSITION, CAMERA
    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        backEnd->displayPosition(backEnd->camera->getViewMatrix());
        backEnd->displayCameraAngles(backEnd->camera->getViewMatrix());
    }
    #endif
};