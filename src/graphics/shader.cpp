#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../configs/math_config.hpp"
#include "shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Shader::Shader(Shader&& other) noexcept
    : m_ID(other.m_ID)
{
    other.m_ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        // delete old GPU program
        glDeleteProgram(m_ID);
        // copy the new ID
        m_ID = other.m_ID;
        other.m_ID = 0;
    }
    return *this;
}

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
        LOG_E("SHADER::READ_FAILED: " << vertexPath << " AND " << fragmentPath);
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    int success;
    char infoLog[512];


    //    ┓┏┏┓┳┓┏┳┓┏┓┏┓┏┓  ┏┓┓┏┏┓┳┓┏┓┳┓
    //    ┃┃┣ ┣┫ ┃ ┣  ┃┃   ┗┓┣┫┣┫┃┃┣ ┣┫
    //    ┗┛┗┛┛┗ ┻ ┗┛┗┛┗┛  ┗┛┛┗┛┗┻┛┗┛┛┗
    //                                 
    m_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(m_vertex, 1, &vShaderCode, NULL);
    glCompileShader(m_vertex);
    glGetShaderiv(m_vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_vertex, 512, NULL, infoLog);
        LOG_E("SHADER::VERTEX::COMPILATION_FAILED: " << vertexPath);
        LOG_E(infoLog);
    };

    //    ┏┓┳┓┏┓┏┓┳┳┓┏┓┳┓┏┳┓  ┏┓┓┏┏┓┳┓┏┓┳┓
    //    ┣ ┣┫┣┫┃┓┃┃┃┣ ┃┃ ┃   ┗┓┣┫┣┫┃┃┣ ┣┫
    //    ┻ ┛┗┛┗┗┛┛ ┗┗┛┛┗ ┻   ┗┛┛┗┛┗┻┛┗┛┛┗
    //                                    
    m_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(m_fragment, 1, &fShaderCode, NULL);
    glCompileShader(m_fragment);
    glGetShaderiv(m_fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_fragment, 512, NULL, infoLog);
        LOG_E("SHADER::FRAGMENT::COMPILATION_FAILED" << fragmentPath);
        LOG_E(infoLog);
    };

    //    ┓ •┳┓┓┏┓•┳┓┏┓
    //    ┃ ┓┃┃┃┫ ┓┃┃┃┓
    //    ┗┛┗┛┗┛┗┛┗┛┗┗┛
    //                 
    m_ID = glCreateProgram();
    glAttachShader(m_ID, m_vertex);
    glAttachShader(m_ID, m_fragment);
    glLinkProgram(m_ID);
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        LOG_E("SHADER::LINKING_FAILED: " << vertexPath << " AND " << fragmentPath);
        LOG_E(infoLog);
    }

    // not used after linking
    glDeleteShader(m_fragment);
    glDeleteShader(m_vertex);
}

Shader::~Shader() {
    // OpenGL ignores glDeleteProgram(0), so no need to check
    glDeleteProgram(m_ID);
}

void Shader::use() const {
    glUseProgram(m_ID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setVec3fv(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4fv(const std::string& name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat3fv(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4fv(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}