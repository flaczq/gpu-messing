#include "mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::draw(Shader &shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (size_t i{}; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        } else if (name == "texture_specular") {
            number = std::to_string(specularNr++);
        } else if (name == "texture_normal") {
            number = std::to_string(normalNr++);
        } else if (name == "texture_height") {
            number = std::to_string(heightNr++);
        }
        shader.setInt("material." + name + number, i);


        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    // state reset
    glActiveTexture(GL_TEXTURE0);

    //    ┳┓┳┓┏┓┓ ┏
    //    ┃┃┣┫┣┫┃┃┃
    //    ┻┛┛┗┛┗┗┻┛
    //             
    glBindVertexArray(VAO);
    // what to render, number of elements, type, offset
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

    // unbind <=> clean up
    glBindVertexArray(0);
}

std::vector<Vertex> Mesh::getVertices() const {
    return vertices;
}
std::vector<unsigned int> Mesh::getIndices() const {
    return indices;
}
std::vector<Texture> Mesh::getTextures() const {
    return textures;
}

void Mesh::setupMesh() {
    //    ┏┓┏┓┏┳┓┳┳┏┓
    //    ┗┓┣  ┃ ┃┃┃┃
    //    ┗┛┗┛ ┻ ┗┛┣┛
    //               
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 1. location in vertex
    // 2. type: vec2/vec3/int/float (2/3/4/4)
    // 3. type: GL_FLOAT/GL_INT
    // 4. normalize
    // 5. stride = difference between each vertex (can be set to 0 if data is packed for auto complete)
    // 6. offset = difference between each location
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    
    // texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // tangent
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
    glEnableVertexAttribArray(3);

    // bitangent
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Bitangent));
    glEnableVertexAttribArray(4);

    // boneIDs
    glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, BoneIDs));
    glEnableVertexAttribArray(5);

    // weights
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Weights));
    glEnableVertexAttribArray(6);

    // unbind <=> clean up
    glBindVertexArray(0);
}