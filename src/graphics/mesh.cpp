#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "graphics_types.hpp"
#include "mesh.h"
#include "shader.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

Mesh::Mesh(Mesh&& other) noexcept
    : m_VAO(other.m_VAO),
      m_VBO(other.m_VBO),
      m_EBO(other.m_EBO),
      m_vertices(std::move(other.m_vertices)),
      m_indices(std::move(other.m_indices)),
      m_textures(std::move(other.m_textures)),
      m_hasDiffuseColor(other.m_hasDiffuseColor),
      m_diffuseColor(other.m_diffuseColor)
{
    // reset IDs to not deconstruct them
    other.m_VAO = 0;
    other.m_VBO = 0;
    other.m_EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        // delete old GPU data
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);

        // copy the new data
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;
        m_EBO = other.m_EBO;

        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_textures = std::move(other.m_textures);
        m_hasDiffuseColor = other.m_hasDiffuseColor;
        m_diffuseColor = other.m_diffuseColor;

        other.m_VAO = 0;
        other.m_VBO = 0;
        other.m_EBO = 0;
    }
    return *this;
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<std::shared_ptr<Texture>>& textures)
    : m_vertices(std::move(vertices)),
    m_indices(std::move(indices)),
    m_textures(std::move(textures)),
    m_hasDiffuseColor(false),
    m_diffuseColor(glm::vec3(1.0f))
{
    setupMesh();
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<std::shared_ptr<Texture>>& textures, bool hasDiffuseColor, glm::vec3 diffuseColor)
    : m_vertices(std::move(vertices)),
      m_indices(std::move(indices)),
      m_textures(std::move(textures)),
      m_hasDiffuseColor(hasDiffuseColor),
      m_diffuseColor(diffuseColor)
{
	setupMesh();
}

Mesh::~Mesh() {
    // OpenGL ignores glDeleteVertexArrays(0) and glDeleteBuffers(0), so no need to check
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
}

void Mesh::draw(Shader& shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (size_t i{}; i < m_textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_textures[i]->type;
        if (name == "diffuse") {
            number = std::to_string(diffuseNr++);
        } else if (name == "specular") {
            number = std::to_string(specularNr++);
        } else if (name == "normal") {
            number = std::to_string(normalNr++);
        } else if (name == "height") {
            number = std::to_string(heightNr++);
        }
        // FIXME more than one of each material type
        //shader.setInt("material." + name + number, i);
        shader.setInt("material." + name + "1", i);

        glBindTexture(GL_TEXTURE_2D, m_textures[i]->id);
    }

    // state reset
    glActiveTexture(GL_TEXTURE0);
    if (m_textures.empty()) {
        // reset binded texture to prevent using one from earlier entity
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (m_hasDiffuseColor) {
        shader.setBool("material.hasDiffuseColor", true);
        shader.setVec3fv("material.diffuseColor", m_diffuseColor);
    }

    //    ┳┓┳┓┏┓┓ ┏•┳┓┏┓
    //    ┃┃┣┫┣┫┃┃┃┓┃┃┃┓
    //    ┻┛┛┗┛┗┗┻┛┗┛┗┗┛
    //                  
    glBindVertexArray(m_VAO);
    // what to render, number of elements, type, offset
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);

    // unbind <=> clean up
    glBindVertexArray(0);
}

void Mesh::setupMesh() {
    //    ┏┓┏┓┏┳┓┳┳┏┓
    //    ┗┓┣  ┃ ┃┃┃┃
    //    ┗┛┗┛ ┻ ┗┛┣┛
    //               
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // 1. location in vertex
    // 2. type: vec2/vec3/int/float (2/3/4/4)
    // 3. type: GL_FLOAT/GL_INT
    // 4. normalize
    // 5. stride = difference between each vertex (can be set to 0 if data is packed for auto complete)
    // 6. offset = difference between each location
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(1);
    
    // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    
    // texture coordinates
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(3);

    // tangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(4);

    // bitangent
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
    glEnableVertexAttribArray(5);

    // boneIDs
    glVertexAttribIPointer(6, 4, GL_INT, sizeof(Vertex), (GLvoid*)offsetof(Vertex, BoneIDs));
    glEnableVertexAttribArray(6);

    // weights
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Weights));
    glEnableVertexAttribArray(7);

    // unbind <=> clean up
    glBindVertexArray(0);
}