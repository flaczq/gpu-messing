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
    Camera camera{};
    camera.init(window);

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
    auto entity = registry.create();
    registry.emplace<Transform>(entity, glm::vec3(0.0f, 0.0f, 0.0f));
}

void Core::run() {
    //    ┓┏┏┓┳┓•┏┓┳┓┓ ┏┓┏┓
    //    ┃┃┣┫┣┫┓┣┫┣┫┃ ┣ ┗┓
    //    ┗┛┛┗┛┗┗┛┗┻┛┗┛┗┛┗┛
    //                     
    glm::vec3 lightPos(1.0f, 2.0f, 3.0f);
    //glm::vec3 lightColor(1.0f);
    float radius = 2.0f;
    // Phong: light = ambient + diffuse + specular

    //    ┳┳┓┏┓•┳┓  ┏┓┏┓┳┳┓┏┓  ┓ ┏┓┏┓┏┓
    //    ┃┃┃┣┫┓┃┃  ┃┓┣┫┃┃┃┣   ┃ ┃┃┃┃┃┃
    //    ┛ ┗┛┗┗┛┗  ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┣┛
    //                                 
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        lightPos.x = cos(currentTime) * radius;
        lightPos.y = sin(currentTime) * radius;

        //lightColor.x = sin(currentTime * 2.0f);
        //lightColor.y = sin(currentTime * 0.7f);
        //lightColor.z = sin(currentTime * 1.3f);

        showFps(window);
        camera.processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //TexturePrimitive::bind(diffuseMapTP, 0);
        //TexturePrimitive::bind(specularMapTP, 1);

        // transformation matrix: clip = projectionM * viewM * modelM * local
        // 1. local * modelM            -> world
        // 2. world * viewM             -> space (lookAt())
        // 3. space * projectionM       -> clip
        // 4. clip  * viewportTransform -> screen

        // transform
        //glm::mat4 transform = glm::mat4(1.0f);
        //transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        //transform = glm::rotate(transform, currentTime, glm::vec3(0.0, 0.0, 1.0));
        //transform = glm::scale(transform, glm::vec3(1.5f, 0.5f, 1.5f));

        // model
        glm::mat4 model = glm::mat4(1.0f);

        // projection
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)screen_w / (float)screen_h, 1.0f, 100.0f);

        // view
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.getViewMatrix();

        // ----------------- object shader ---------------- //
        /*objectShader->use();
        glBindVertexArray(VAO);

        objectShader->setMat4fv("projection", projection);
        objectShader->setMat4fv("view", view);

        objectShader->setBool("spotlightOn", std::get<bool>(uniformVars["spotlightOn"]));
        objectShader->setVec3fv("viewPos", camera.getPosition());

        objectShader->setInt("material.diffuse", 0);
        objectShader->setInt("material.specular", 1);
        objectShader->setFloat("material.shininess", 64.0f);

        objectShader->setVec3fv("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        objectShader->setVec3fv("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        objectShader->setVec3fv("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        objectShader->setVec3fv("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

        objectShader->setVec3fv("pointLights[0].position", pointLightPositions[0] + lightPos);
        objectShader->setVec3fv("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        objectShader->setVec3fv("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        objectShader->setVec3fv("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setFloat("pointLights[0].constant", 1.0f);
        objectShader->setFloat("pointLights[0].linear", 0.09f);
        objectShader->setFloat("pointLights[0].quadratic", 0.032f);
        objectShader->setVec3fv("pointLights[1].position", pointLightPositions[1] + lightPos);
        objectShader->setVec3fv("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        objectShader->setVec3fv("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        objectShader->setVec3fv("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setFloat("pointLights[1].constant", 1.0f);
        objectShader->setFloat("pointLights[1].linear", 0.09f);
        objectShader->setFloat("pointLights[1].quadratic", 0.032f);
        objectShader->setVec3fv("pointLights[2].position", pointLightPositions[2] + lightPos);
        objectShader->setVec3fv("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        objectShader->setVec3fv("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        objectShader->setVec3fv("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setFloat("pointLights[2].constant", 1.0f);
        objectShader->setFloat("pointLights[2].linear", 0.09f);
        objectShader->setFloat("pointLights[2].quadratic", 0.032f);
        objectShader->setVec3fv("pointLights[3].position", pointLightPositions[3] + lightPos);
        objectShader->setVec3fv("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        objectShader->setVec3fv("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        objectShader->setVec3fv("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setFloat("pointLights[3].constant", 1.0f);
        objectShader->setFloat("pointLights[3].linear", 0.09f);
        objectShader->setFloat("pointLights[3].quadratic", 0.032f);

        objectShader->setVec3fv("spotlight.position", camera.getPosition());
        objectShader->setVec3fv("spotlight.direction", camera.getFront());
        objectShader->setFloat("spotlight.cutOff", glm::cos(glm::radians(12.5f)));
        objectShader->setFloat("spotlight.outerCutOff", glm::cos(glm::radians(15.0f)));
        objectShader->setVec3fv("spotlight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        objectShader->setVec3fv("spotlight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setVec3fv("spotlight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        objectShader->setFloat("spotlight.constant", 1.0f);
        objectShader->setFloat("spotlight.linear", 0.09f);
        objectShader->setFloat("spotlight.quadratic", 0.032f);

        for (size_t i{}; i < cubePositions.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            objectShader->setMat4fv("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/
        // ------------------------------------------------ //

        // ----------------- light shader ----------------- //
        /*lightShader->use();
        glBindVertexArray(lightVAO);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 3.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, currentTime * glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader->setMat4fv("model", model);
        lightShader->setMat4fv("projection", projection);
        lightShader->setMat4fv("view", view);
        glDrawArrays(GL_TRIANGLES, 0, 36);*/
        /*for (size_t i{}; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i] + lightPos);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader->setMat4fv("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/
        // ------------------------------------------------ //

        // ------------------ grid shader ----------------- //
        gridShader->use();
        glBindVertexArray(gridVAO);

        model = glm::mat4(1.0f);
        gridShader->setMat4fv("model", model);
        gridShader->setMat4fv("projection", projection);
        gridShader->setMat4fv("view", view);

        glDisable(GL_DEPTH_TEST);
        // antialiasing
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, gridPositions.size());
        //glLineWidth(1.0f);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        // ------------------------------------------------ //

        // ----------------- gizmo shader ----------------- //
        gizmoShader->use();
        glBindVertexArray(gizmoVAO);

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(std::get<float>(uniformVars["gizmoLength"])));
        gizmoShader->setBool("gizmoNegative", std::get<bool>(uniformVars["gizmoNegative"]));
        gizmoShader->setMat4fv("model", model);
        gizmoShader->setMat4fv("projection", projection);
        gizmoShader->setMat4fv("view", view);

        glDisable(GL_DEPTH_TEST);
        // antialiasing
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, 6);
        //glLineWidth(1.0f);
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        // ------------------------------------------------ //

        // ----------------- model shader ----------------- //
        modelShader->use();

        // scale fix and optimization with CPU computation
        model = glm::mat4(1.0f);
        modelShader->setMat3fv("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
        modelShader->setMat4fv("projection", projection);
        modelShader->setMat4fv("view", view);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 3.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, currentTime * glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(100.0f));
        modelShader->setMat4fv("model", model);

        modelShader->setVec3fv("lightDir", glm::vec3(0.5f, -1.0f, -0.5f));
        modelShader->setVec3fv("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        modelShader->setVec3fv("viewPos", camera.getPosition());

        modelSoldier->draw(*modelShader);
        // ------------------------------------------------ //

        // no need to unbind it every time but w/e
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

Camera& Core::getCamera() {
    return camera;
}

Renderer& Core::getRenderer() {
    return renderer;
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
    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
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
    }
};