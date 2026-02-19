#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    std::cout << "Window changed to " << width << 'x' << height << std::endl;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Cannot open file: " << filepath << std::endl;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    //    ┏┳┓┓┏•┏┓  •┏┓  •┏┳┓
    //     ┃ ┣┫┓┗┓  ┓┗┓  ┓ ┃ 
    //     ┻ ┛┗┗┗┛  ┗┗┛  ┗ ┻ 
    //                       
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "(C) Uknnowndsn gameengine 2049", nullptr, nullptr);
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

    if (gl3wInit()) {
        std::cerr << "Failed to init GL3W" << std::endl;
        //glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //    ┓┏┏┓┳┓┏┳┓┏┓┏┓┏┓  ┏┓┓┏┏┓┳┓┏┓┳┓
    //    ┃┃┣ ┣┫ ┃ ┣  ┃┃   ┗┓┣┫┣┫┃┃┣ ┣┫
    //    ┗┛┗┛┛┗ ┻ ┗┛┗┛┗┛  ┗┛┛┗┛┗┻┛┗┛┛┗
    //                                 
    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLint vertexShaderSuccess;
    std::string vertexTriangleFile = loadShaderSource("triangle.vert");
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderSuccess);
    if (!vertexShaderSuccess) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader error: " << infoLog << std::endl;
    }
    const char* vertexTriangleSource = vertexTriangleFile.c_str();
    glShaderSource(vertexShader, 1, &vertexTriangleSource, NULL);
    glCompileShader(vertexShader);

    GLint vertexShaderCompSuccess;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompSuccess);
    if (!vertexShaderCompSuccess) {
        GLchar infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //    ┏┓┳┓┏┓┏┓┳┳┓┏┓┳┓┏┳┓  ┏┓┓┏┏┓┳┓┏┓┳┓
    //    ┣ ┣┫┣┫┃┓┃┃┃┣ ┃┃ ┃   ┗┓┣┫┣┫┃┃┣ ┣┫
    //    ┻ ┛┗┛┗┗┛┛ ┗┗┛┛┗ ┻   ┗┛┛┗┛┗┻┛┗┛┛┗
    //                                    
    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint fragmentShaderSuccess;
    std::string fragmentTriangleFile = loadShaderSource("triangle.frag");
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderSuccess);
    if (!fragmentShaderSuccess) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader error: " << infoLog << std::endl;
    }
    const char* fragmentTriangleSource = fragmentTriangleFile.c_str();
    glShaderSource(fragmentShader, 1, &fragmentTriangleSource, NULL);
    glCompileShader(fragmentShader);

    GLint fragmentShaderCompSuccess;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompSuccess);
    if (!fragmentShaderCompSuccess) {
        GLchar infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //    ┓ •┳┓┓┏┓•┳┓┏┓
    //    ┃ ┓┃┃┃┫ ┓┃┃┃┓
    //    ┗┛┗┛┗┛┗┛┗┛┗┗┛
    //                 
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint shaderProgramSuccess;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &shaderProgramSuccess);
    if (!shaderProgramSuccess) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //    ┏┓┓ ┏┓┏┓┳┓•┳┓┏┓
    //    ┃ ┃ ┣ ┣┫┃┃┓┃┃┃┓
    //    ┗┛┗┛┗┛┛┗┛┗┗┛┗┗┛
    //                   
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //    •┳┓┏┓┳┳┏┳┓  ┳┓┏┓┏┳┓┏┓
    //    ┓┃┃┃┃┃┃ ┃   ┃┃┣┫ ┃ ┣┫
    //    ┗┛┗┣┛┗┛ ┻   ┻┛┛┗ ┻ ┛┗
    //                         
    GLfloat vertices[] = {
        0.5f,  0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f
    };
    GLfloat triangle1[] = {
        -0.9f, -0.5f, 0.0f,
        -0.0f, -0.5f, 0.0f,
        -0.45f, 0.5f, 0.0f
    };
    GLfloat triangle2[] = {
        0.0f, -0.5f, 0.0f,
        0.9f, -0.5f, 0.0f,
        0.45f, 0.5f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //    ┳┓┏┓┳┓┳┓┏┓┳┓•┳┓┏┓
    //    ┣┫┣ ┃┃┃┃┣ ┣┫┓┃┃┃┓
    //    ┛┗┗┛┛┗┻┛┗┛┛┗┗┛┗┗┛
    //                     
    GLuint VAOs[2], VBOs[2];// , EBO;
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    //glGenBuffers(1, &EBO);

    // first triangle config
    glBindVertexArray(VBOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // second triangle config
    glBindVertexArray(VBOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // no need to unbind at all as we directly bind a different VAO the next few lines
    //glBindVertexArray(0);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //    ┓┏┏┓┳┓┏┳┓•┏┓┏┓┏┓  ┏┓┏┓┳┓┏┓•┏┓
    //    ┃┃┣ ┣┫ ┃ ┓┃ ┣ ┗┓  ┃ ┃┃┃┃┣ ┓┃┓
    //    ┗┛┗┛┛┗ ┻ ┗┗┛┗┛┗┛  ┗┛┗┛┛┗┻ ┗┗┛
    //                     
    // 1. location in vertex
    // 2. vec3
    // 3. GL_FLOAT (vec3)
    // 4. normalize
    // 5. stride = difference between each data (can be set to 0 if data is packed for auto complete)
    // 6. offset (dw)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //    ┳┳┓┏┓•┳┓  ┏┓┏┓┳┳┓┏┓  ┓ ┏┓┏┓┏┓
    //    ┃┃┃┣┫┓┃┃  ┃┓┣┫┃┃┃┣   ┃ ┃┃┃┃┃┃
    //    ┛ ┗┛┗┗┛┗  ┗┛┛┗┛ ┗┗┛  ┗┛┗┛┗┛┣┛
    //                                 
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        // draw the first triangle from the first VAO
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // draw the second triangle from the second VAO
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // no need to unbind it every time
        //glBindVertexArray(0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    //    ┏┓┓ ┏┓┏┓┳┓•┳┓┏┓
    //    ┃ ┃ ┣ ┣┫┃┃┓┃┃┃┓
    //    ┗┛┗┛┗┛┛┗┛┗┗┛┗┗┛
    //           
    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    //glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}