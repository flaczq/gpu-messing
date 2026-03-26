#include "core.h"

Core::Core(int w, int h) {
    screen_w = w;
    screen_h = h;

    objectShader = nullptr;
    lightShader = nullptr;
    gridShader = nullptr;
    gizmoShader = nullptr;
}

Core::~Core() {
    // simple data structures (not meshes)
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &gridVAO);
	glDeleteVertexArrays(1, &gizmoVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &gizmoVBO);
	glDeleteBuffers(1, &gridVBO);
    
    // force deleting the shaders -> shader deletes its ID nad unique_ptr deletes object itself
    objectShader.reset();
    lightShader.reset();
    gridShader.reset();
    gizmoShader.reset();
    modelShader.reset();

    //TexturePrimitive::clean(diffuseMapTP);
    //TexturePrimitive::clean(specularMapTP);

    glfwTerminate();
}

bool Core::init() {
    //    ┏┳┓┓┏•┏┓  •┏┓  •┏┳┓
    //     ┃ ┣┫┓┗┓  ┓┗┓  ┓ ┃ 
    //     ┻ ┛┗┗┗┛  ┗┗┛  ┗ ┻ 
    //                       
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_w, screen_h, "(C) Engine Runner 2049", nullptr, nullptr);
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

    // ONLY ONCE set 'this' as Core
    glfwSetWindowUserPointer(window, this);

    //    ┏┓┓┏┏┓┳┓┏┓┳┓┏┓
    //    ┗┓┣┫┣┫┃┃┣ ┣┫┗┓
    //    ┗┛┛┗┛┗┻┛┗┛┛┗┗┛
    //                  
    objectShader = std::make_unique<Shader>("../shaders/object.vert", "../shaders/object.frag");
    lightShader = std::make_unique<Shader>("../shaders/light.vert", "../shaders/light.frag");
    gridShader = std::make_unique<Shader>("../shaders/grid.vert", "../shaders/grid.frag");
    gizmoShader = std::make_unique<Shader>("../shaders/gizmo.vert", "../shaders/gizmo.frag");
    modelShader = std::make_unique<Shader>("../shaders/model.vert", "../shaders/model.frag");

    //    ┳┳┓┏┓┳┓┏┓┓   ┏┓  ┳┳┓┏┓┏┓┓┏
    //    ┃┃┃┃┃┃┃┣ ┃   ┣╋  ┃┃┃┣ ┗┓┣┫
    //    ┛ ┗┗┛┻┛┗┛┗┛  ┗┻  ┛ ┗┗┛┗┛┛┗
    //                              
    modelSoldier = std::make_unique<Model>("../assets/models/Soldier.glb");

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓  ┏┓┳┓•┳┳┓┳┏┳┓•┓┏┏┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣   ┃┃┣┫┓┃┃┃┃ ┃ ┓┃┃┣ ┗┓
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛  ┣┛┛┗┗┛ ┗┻ ┻ ┗┗┛┗┛┗┛
    //                                           
    //diffuseMapTP = TexturePrimitive::load("../assets/container2.png");
    //specularMapTP = TexturePrimitive::load("../assets/container2_specular.png");

    //    ┳┓┏┓┳┓┳┓┏┓┳┓┏┓┳┓
    //    ┣┫┣ ┃┃┃┃┣ ┣┫┣ ┣┫
    //    ┛┗┗┛┛┗┻┛┗┛┛┗┗┛┛┗
    //                    
    Renderer renderer{};
    //renderer.init();

    //    ┏┓┏┓┳┳┓┏┓┳┓┏┓
    //    ┃ ┣┫┃┃┃┣ ┣┫┣┫
    //    ┗┛┛┗┛ ┗┗┛┛┗┛┗
    //                 
    //Camera camera{};
    //camera.init(window);

    //    ┳┓┏┓┏┳┓┏┓  ┏┓┳┓•┳┳┓┳┏┳┓•┓┏┏┓┏┓
    //    ┃┃┣┫ ┃ ┣┫  ┃┃┣┫┓┃┃┃┃ ┃ ┓┃┃┣ ┗┓
    //    ┻┛┛┗ ┻ ┛┗  ┣┛┛┗┗┛ ┗┻ ┻ ┗┗┛┗┛┗┛
    //                                  
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    float gizmo[] = {
        // positions        // colors
        -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,

         0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

         0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 1.0f
    };
    unsigned int gS = 10u;
    for (size_t i{}; i <= gS; i++) {
        float pos = (float)i;
        // z axis
        gridPositions.push_back(glm::vec3(pos, 0.0f, 0.0f));
        gridPositions.push_back(glm::vec3(pos, 0.0f, (float)gS));
        // x axis
        gridPositions.push_back(glm::vec3(0.0f, 0.0f, pos));
        gridPositions.push_back(glm::vec3((float)gS, 0.0f, pos));
    }
    cubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    cubePositions.push_back(glm::vec3(2.0f, 5.0f, -15.0f));
    cubePositions.push_back(glm::vec3(-1.5f, -2.2f, -2.5f));
    cubePositions.push_back(glm::vec3(-3.8f, -2.0f, -12.3f));
    cubePositions.push_back(glm::vec3(2.4f, -0.4f, -3.5f));
    cubePositions.push_back(glm::vec3(-1.7f, 3.0f, -7.5f));
    cubePositions.push_back(glm::vec3(1.3f, -2.0f, -2.5f));
    cubePositions.push_back(glm::vec3(1.5f, 2.0f, -2.5f));
    cubePositions.push_back(glm::vec3(1.5f, 0.2f, -1.5f));
    cubePositions.push_back(glm::vec3(-1.3f, 1.0f, -1.5f));
    pointLightPositions.push_back(glm::vec3(0.7f, 0.2f, 2.0f));
    pointLightPositions.push_back(glm::vec3(2.3f, -3.3f, -4.0f));
    pointLightPositions.push_back(glm::vec3(-4.0f, 2.0f, -12.0f));
    pointLightPositions.push_back(glm::vec3(0.0f, 0.0f, -3.0f));

    //    ┓┏┏┓┳┓┏┳┓•┏┓┏┓┏┓
    //    ┃┃┣ ┣┫ ┃ ┓┃ ┣ ┗┓
    //    ┗┛┗┛┛┗ ┻ ┗┗┛┗┛┗┛
    //                    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 1. location in vertex
    // 2. vec2/vec3
    // 3. GL_FLOAT (vec3)
    // 4. normalize
    // 5. stride = difference between each vertex (can be set to 0 if data is packed for auto complete)
    // 6. offset = difference between each location
    // -------------------- VAO ------------------- //
    glBindVertexArray(VAO);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    // -------------------------------------------- //

    // ----------------- lightVAO ----------------- //
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // same VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // -------------------------------------------- //

    // ------------------ gridVAO ----------------- //
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridPositions.size() * sizeof(glm::vec3), gridPositions.data(), GL_STATIC_DRAW);
    glBindVertexArray(gridVAO);
    // gridVBO
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    // position as vec3
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // -------------------------------------------- //

    // ----------------- gizmoVAO ----------------- //
    glGenVertexArrays(1, &gizmoVAO);
    glGenBuffers(1, &gizmoVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gizmo), gizmo, GL_STATIC_DRAW);
    glBindVertexArray(gizmoVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gizmoVBO);
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // antialiasing
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // -------------------------------------------- //

    // no need to unbind at all as we directly bind a different VAO the next few lines
    glBindVertexArray(0);

    // standard, lines (wireframe), points
    glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(renderer.getRenderMode()));

    // set callbacks: window resize, single key click
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    return 1;
}

void Core::setup() {
    //    ┏┓┏┓┏┓
    //    ┣ ┃ ┗┓
    //    ┗┛┗┛┗┛
    //          
    for (size_t i{}; i < 10; i++) {
        auto entity = registry.create();
        registry.emplace<TransformComponent>(entity, glm::vec3(i * 2.0f, 0.0f, 0.0f));
    }
}

void Core::run() {
    auto transformComponents = registry.view<TransformComponent>();
    for (auto transformEntity : transformComponents) {
        auto& transform = transformComponents.get<TransformComponent>(transformEntity);
        std::cout << glm::to_string(transform.position) << std::endl;
        std::cout << glm::to_string(transform.rotiation) << std::endl;
        std::cout << glm::to_string(transform.scale) << std::endl;
    }
}

void Core::showFps(GLFWwindow* window) {
	double currentTime = glfwGetTime();
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

void Core::displayPosition(glm::mat4 viewMatrix) {
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec3 pos = glm::vec3(inverseView[3]);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "* Position Status" << std::endl;
    std::cout << "X: " << std::showpos << pos.x << "   "
              << "Y: " << std::showpos << pos.y << "   "
              << "Z: " << std::showpos << pos.z << std::endl << std::endl;
}

void Core::displayCameraAngles(glm::mat4 viewMatrix) {
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

void Core::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// single key click
void Core::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Core* core = static_cast<Core*>(glfwGetWindowUserPointer(window));

    // INTERPOLATE mix of textures (not used)
    float& interpolate = std::get<float>(core->uniformVars["interpolate"]);
    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        interpolate += 0.1f;
    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        interpolate -= 0.1f;
    }
    interpolate = std::clamp(interpolate, 0.0f, 1.0f);

    // GOD MODE
    /*if (key == GLFW_KEY_G && action == GLFW_PRESS) {
        core->camera.changeGodMode();
    }

    // CROUCHING/STANDING
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        core->camera.changeCameraMode();
    }

    // SPOTLIGHT
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        bool& spotlightOn = std::get<bool>(core->uniformVars["spotlightOn"]);
        spotlightOn = !spotlightOn;
        std::cout << "* Changed spotlight to: " << std::boolalpha << spotlightOn << std::endl << std::endl;
    }
    
    // RENDER MODE
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        std::string renderModeStr;
        RenderMode renderMode = core->renderer.getRenderMode();
        if (renderMode == RenderMode::STANDARD) {
            renderMode = RenderMode::WIREFRAME;
            renderModeStr = "WIREFRAME";
        }
        else if (renderMode == RenderMode::WIREFRAME) {
            renderMode = RenderMode::POINTCLOUD;
            renderModeStr = "POINTCLOUD";
        }
        else {
            renderMode = RenderMode::STANDARD;
            renderModeStr = "STANDARD";
        }
        core->renderer.setRenderMode(renderMode);
        glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(renderMode));
        std::cout << "* Changed RenderMode to: " << renderModeStr << std::endl << std::endl;
    }
    
    // GIZMO LENGTH
    float& gizmoLength = std::get<float>(core->uniformVars["gizmoLength"]);
    if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS) {
        gizmoLength += 1.0f;
    }
    if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS) {
        gizmoLength -= 1.0f;
    }
    gizmoLength = std::clamp(gizmoLength, 0.0f, 20.0f);

    // GIZMO NEGATIVE AXIS
    bool& gizmoNegative = std::get<bool>(core->uniformVars["gizmoNegative"]);
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        gizmoNegative = !gizmoNegative;
    }

    // INFO: POSITION, CAMERA
    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        core->displayPosition(core->camera.getViewMatrix());
        core->displayCameraAngles(core->camera.getViewMatrix());
    }*/
};