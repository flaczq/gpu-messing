#include "engine.h"

Engine::Engine(int w, int h) : screen_w(w), screen_h(h), objectShader(nullptr) {
}
Engine::~Engine() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
    glDeleteProgram(objectShader->ID);

	glfwTerminate();
}

bool Engine::init() {
    //    ┏┳┓┓┏•┏┓  •┏┓  •┏┳┓
    //     ┃ ┣┫┓┗┓  ┓┗┓  ┓ ┃ 
    //     ┻ ┛┗┗┗┛  ┗┗┛  ┗ ┻ 
    //                       
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screen_w, screen_h, "(C) Uknnowndsn gameengine 2049", nullptr, nullptr);
    std::cout << R"(
     ## you look lonely, i can fix that ##
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

    // ONLY ONCE set 'this' as Engine
    glfwSetWindowUserPointer(window, this);

    //    ┏┓┏┓┳┳┓┏┓┳┓┏┓
    //    ┃ ┣┫┃┃┃┣ ┣┫┣┫
    //    ┗┛┛┗┛ ┗┗┛┛┗┛┗
    //                 
    Camera camera;
    camera.init(window);

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣ ┗┓
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛┗┛
    //                        
    glGenTextures(1, &diffuseMap);
    int data_w, data_h, data_ch;
    unsigned char* data = stbi_load("assets/container2.png", &data_w, &data_h, &data_ch, 0);
    if (data) {
        GLenum format;
        if (data_ch == 1) {
            format = GL_RED;
        } else if (data_ch == 3) {
            format = GL_RGB;
        } else if (data_ch == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glTexImage2D(GL_TEXTURE_2D, 0, format, data_w, data_h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // filter parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "ERROR::MAIN::DIFFUSE_MAP_NOT_LOADED_SUCCESFULLY" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &specularMap);
    data = stbi_load("assets/container2_specular.png", &data_w, &data_h, &data_ch, 0);
    if (data) {
        GLenum format;
        if (data_ch == 1) {
            format = GL_RED;
        }
        else if (data_ch == 3) {
            format = GL_RGB;
        }
        else if (data_ch == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, specularMap);
        glTexImage2D(GL_TEXTURE_2D, 0, format, data_w, data_h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // filter parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "ERROR::MAIN::SPECULAR_MAP_NOT_LOADED_SUCCESFULLY" << std::endl;
    }
    stbi_image_free(data);

    // active shaders
    objectShader = new Shader("shaders/object.vert", "shaders/object.frag");
    lightShader = new Shader("shaders/light.vert", "shaders/light.frag");

    //    •┳┓┏┓┳┳┏┳┓  ┳┓┏┓┏┳┓┏┓
    //    ┓┃┃┃┃┃┃ ┃   ┃┃┣┫ ┃ ┣┫
    //    ┗┛┗┣┛┗┛ ┻   ┻┛┛┗ ┻ ┛┗
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

    //    ┓┏┏┓┳┓┏┳┓•┏┓┏┓┏┓
    //    ┃┃┣ ┣┫ ┃ ┓┃ ┣ ┗┓
    //    ┗┛┗┛┛┗ ┻ ┗┗┛┗┛┗┛
    //                    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 1. location in vertex
    // 2. vec2/vec3
    // 3. GL_FLOAT (vec3)
    // 4. normalize
    // 5. stride = difference between each vertex (can be set to 0 if data is packed for auto complete)
    // 6. offset = difference between each location
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

    // lightVAO
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // same VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // no need to unbind at all as we directly bind a different VAO the next few lines
    //glBindVertexArray(0);

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// set callbacks: window resize, single key click
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

	return 1;
}

void Engine::run() {
    //    ┓┏┏┓┳┓•┏┓┳┓┓ ┏┓┏┓
    //    ┃┃┣┫┣┫┓┣┫┣┫┃ ┣ ┗┓
    //    ┗┛┛┗┛┗┗┛┗┻┛┗┛┗┛┗┛
    //                     
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    //glm::vec3 lightColor(1.0f);
    float radius = 2.0f;
    // Phong: light = ambient + diffuse + specular

    //    ┳┳┓┏┓•┳┓  ┏┓┏┓┳┳┓┏┓  ┓ ┏┓┏┓┏┓
    //    ┃┃┃┣┫┓┃┃  ┃┓┣┫┃┃┃┣   ┃ ┃┃┃┃┃┃
    //    ┛ ┗┛┗┗┛┗  ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┣┛
    //                                 
    while (!glfwWindowShouldClose(window)) {
        showFps(window);

        camera.processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLfloat currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        lightPos.x = cos(currentTime) * radius;
        lightPos.y = sin(currentTime) * radius;

        //lightColor.x = sin(currentTime * 2.0f);
        //lightColor.y = sin(currentTime * 0.7f);
        //lightColor.z = sin(currentTime * 1.3f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

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

        // projection
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.getZoom()), (float)screen_w / (float)screen_h, 1.0f, 100.0f);

        // view
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.getViewMatrix();

        // model
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::rotate(model, currentTime * glm::radians(13.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // ----------------- object shader ----------------- //
        objectShader->use();
        //objectShader->setMat4fv("transform", transform);
        objectShader->setMat4fv("projection", projection);
        objectShader->setMat4fv("view", view);
        objectShader->setMat4fv("model", model);
        //objectShader->setFloat("interpolate", uniformInterpolate);

        // fragment
        objectShader->setVec3fv("viewPos", camera.getPosition());
        objectShader->setVec3fv("light.position", lightPos);

        objectShader->setInt("material.diffuse", 0);
        objectShader->setInt("material.specular", 1);
        objectShader->setFloat("material.shininess", 64.0f);

        objectShader->setVec3fv("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        objectShader->setVec3fv("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        objectShader->setVec3fv("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ----------------- light shader ----------------- //
        lightShader->use();

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightShader->setMat4fv("projection", projection);
        lightShader->setMat4fv("view", view);
        lightShader->setMat4fv("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // no need to unbind it every time
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

Camera& Engine::getCamera() {
    return camera;
}

void Engine::showFps(GLFWwindow* window) {
	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) {
		double fps = double(nbFrames);
		double msPerFrame = 1000.0 / double(nbFrames);
		std::string title = "(C) Uknnowndsn gameengine 2049 - FPS: " + std::to_string((int)fps) + " (" + std::to_string(msPerFrame).substr(0, 4) + " ms)";
		glfwSetWindowTitle(window, title.c_str());
		nbFrames = 0;
		lastTime += 1.0;
	}
}

void Engine::displayPosition(glm::mat4 viewMatrix) {
    glm::mat4 inverseView = glm::inverse(viewMatrix);
    glm::vec3 pos = glm::vec3(inverseView[3]);

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "------- Position Status ------" << std::endl;
    std::cout << "X: " << std::showpos << pos.x << "   "
              << "Y: " << std::showpos << pos.y << "   "
              << "Z: " << std::showpos << pos.z << std::endl << std::endl;
}

void Engine::displayCameraAngles(glm::mat4 viewMatrix) {
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
        std::cout << "------------------ Camera Status ------------------" << std::endl;
        std::cout << "Pitch: " << std::showpos << pitch << " deg   "
                  << "Yaw: " << std::showpos << yaw << " deg   "
                  << "Roll: " << std::showpos << roll << " deg" << std::endl << std::endl;
    }
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

    // interpolate mix of textures
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        engine->uniformInterpolate += 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        engine->uniformInterpolate -= 0.1f;
    }

    if (engine->uniformInterpolate < 0.0f) {
        engine->uniformInterpolate = 0.0f;
    }
    if (engine->uniformInterpolate > 1.0f) {
        engine->uniformInterpolate = 1.0f;
    }

    // INFO
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        engine->displayPosition(engine->camera.getViewMatrix());
        engine->displayCameraAngles(engine->camera.getViewMatrix());
    }
};