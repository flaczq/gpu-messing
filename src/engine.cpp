#include "engine.h"

Engine::Engine(int w, int h) : screen_w(w), screen_h(h), shader(nullptr) {
}
Engine::~Engine() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader->ID);

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

    if (gl3wInit()) {
        std::cerr << "Failed to init GL3W" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    //    ┏┳┓┏┓┏┓┏┓┏┳┓┳┳┳┓┏┓
    //     ┃ ┣  ┃┃  ┃ ┃┃┣┫┣ 
    //     ┻ ┗┛┗┛┗┛ ┻ ┗┛┛┗┗┛
    //    
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int data_w, data_h, data_ch;
    unsigned char* data = stbi_load("assets/wall.jpg", &data_w, &data_h, &data_ch, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data_w, data_h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ERROR::MAIN::WALL_NOT_LOADED_SUCCESFULLY" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("assets/potato.jpg", &data_w, &data_h, &data_ch, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data_w, data_h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "ERROR::MAIN::FACE_NOT_LOADED_SUCCESFULLY" << std::endl;
    }
    stbi_image_free(data);

    // active shaders
    shader = new Shader("shaders/triangle.vert", "shaders/triangle.frag");

    shader->use();
    shader->setInt("texture1", 0);
    shader->setInt("texture2", 1);
    shader->setFloat("interpolate", uniformInterpolate);

    //    •┳┓┏┓┳┳┏┳┓  ┳┓┏┓┏┳┓┏┓
    //    ┓┃┃┃┃┃┃ ┃   ┃┃┣┫ ┃ ┣┫
    //    ┗┛┗┣┛┗┛ ┻   ┻┛┛┗ ┻ ┛┗
    //                         
    GLfloat vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //    ┳┓┏┓┏┳┓┏┓•┳┓┏┓
    //    ┃┃┣┫ ┃ ┣┫┓┃┃┃┓
    //    ┻┛┛┗ ┻ ┛┗┗┛┗┗┛
    //                  
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // first triangle config
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // no need to unbind at all as we directly bind a different VAO the next few lines
    //glBindVertexArray(0);

    //    ┓┏┏┓┳┓┏┳┓•┏┓┏┓┏┓  ┏┓┏┓┳┓┏┓•┏┓
    //    ┃┃┣ ┣┫ ┃ ┓┃ ┣ ┗┓  ┃ ┃┃┃┃┣ ┓┃┓
    //    ┗┛┗┛┛┗ ┻ ┗┗┛┗┛┗┛  ┗┛┗┛┛┗┻ ┗┗┛
    //                     
    // 1. location in vertex
    // 2. vec3
    // 3. GL_FLOAT (vec3)
    // 4. normalize
    // 5. stride = difference between each vertex (can be set to 0 if data is packed for auto complete)
    // 6. offset = difference between each location
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    // unbind VAO
    glBindVertexArray(0);

    // wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// set callbacks
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return 1;
}

void Engine::run() {
    //    ┳┳┓┏┓•┳┓  ┏┓┏┓┳┳┓┏┓  ┓ ┏┓┏┓┏┓
    //    ┃┃┃┣┫┓┃┃  ┃┓┣┫┃┃┃┣   ┃ ┃┃┃┃┃┃
    //    ┛ ┗┛┗┗┛┗  ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┣┛
    //                                 
    while (!glfwWindowShouldClose(window)) {
        showFps(window);
        processInput(window);

        // set 'this' as Engine
        glfwSetWindowUserPointer(window, this);
        // set callbacks
        glfwSetKeyCallback(window, key_callback);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // transform objects
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        transform = glm::scale(transform, glm::vec3(1.5f, 0.5f, 1.5f));

        // transformation matrix: Vclip = Mprojection * Mview * Mmodel * Vlocal
        // 1. local + Mmodel      -> world
        // 2. world + Mview       -> space
        // 3. space + Mprojection -> clip
        // 4. clip  + Tviewport   -> screen

        // 1. local to world
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // set scene back a little to see everything
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // projection (45 FOV)
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)screen_w / (float)screen_h, 1.0f, 100.0f);

        // send changed variable
        shader->setMatrix4fv("transform", transform);
        shader->setMatrix4fv("model", model);
        shader->setMatrix4fv("view", view);
        shader->setMatrix4fv("projection", projection);

        // draw first triangles
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // second transformers
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
        GLfloat scaleAmount = sin(glfwGetTime());
        transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        shader->setMatrix4fv("transform", transform);

        // draw second triangles
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // update changes
        shader->setFloat("interpolate", uniformInterpolate);

        // no need to unbind it every time
        glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
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

void Engine::processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void Engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
	bool interpolateChanged = false;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && engine->uniformInterpolate < 1.0f) {
		interpolateChanged = true;
        engine->uniformInterpolate += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && engine->uniformInterpolate > 0.0f) {
		interpolateChanged = true;
        engine->uniformInterpolate -= 0.1f;
	}

	if (interpolateChanged) {
		if (engine->uniformInterpolate < 0.0f) {
            engine->uniformInterpolate = 0.0f;
		}
		if (engine->uniformInterpolate > 1.0f) {
            engine->uniformInterpolate = 1.0f;
		}
		std::cout << "uniformInterpolate: " << engine->uniformInterpolate << std::endl;
	}
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	std::cout << "Window changed to " << width << 'x' << height << std::endl;
	glViewport(0, 0, width, height);
}