#include "shader.h"

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    //    ┏┓•┓ ┏┓  ┓ ┏┓┏┓┳┓•┳┓┏┓
    //    ┣ ┓┃ ┣   ┃ ┃┃┣┫┃┃┓┃┃┃┓
    //    ┻ ┗┗┛┗┛  ┗┛┗┛┛┗┻┛┗┛┗┗┛
    //                          
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR::SHADER::NOT_READ_SUCCESFULLY: " << vertexPath << " / " << fragmentPath << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    int success;
    char infoLog[512];


    //    ┓┏┏┓┳┓┏┳┓┏┓┏┓┏┓  ┏┓┓┏┏┓┳┓┏┓┳┓
    //    ┃┃┣ ┣┫ ┃ ┣  ┃┃   ┗┓┣┫┣┫┃┃┣ ┣┫
    //    ┗┛┗┛┛┗ ┻ ┗┛┗┛┗┛  ┗┛┛┗┛┗┻┛┗┛┛┗
    //                                 
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED: " << vertexPath << "\n" << infoLog << std::endl;
    };

    //    ┏┓┳┓┏┓┏┓┳┳┓┏┓┳┓┏┳┓  ┏┓┓┏┏┓┳┓┏┓┳┓
    //    ┣ ┣┫┣┫┃┓┃┃┃┣ ┃┃ ┃   ┗┓┣┫┣┫┃┃┣ ┣┫
    //    ┻ ┛┗┛┗┗┛┛ ┗┗┛┛┗ ┻   ┗┛┛┗┛┗┻┛┗┛┛┗
    //                                    
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED" << fragmentPath << "\n" << infoLog << std::endl;
    };

    //    ┓ •┳┓┓┏┓•┳┓┏┓
    //    ┃ ┓┃┃┃┫ ┓┃┃┃┓
    //    ┗┛┗┛┗┛┗┛┗┛┗┗┛
    //                 
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINKING_FAILED: " << vertexPath << " / " << fragmentPath << "\n" << infoLog << std::endl;
    }

    // not used after linking
    glDeleteShader(fragment);
    glDeleteShader(vertex);
}
Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setVec3fv(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4fv(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setMat4fv(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}