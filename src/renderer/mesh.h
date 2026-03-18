#pragma once

#define MAX_BONE_INFLUENCE 4

#include "../utils/commongl.h"
#include "../renderer/shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int BoneIDs[MAX_BONE_INFLUENCE];
	float Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	// move constructor
	Mesh(Mesh&& other) noexcept;
	// block copying
	Mesh(const Mesh&) = delete;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();

	void draw(Shader &shader);

	std::vector<Vertex> getVertices() const;
	std::vector<unsigned int> getIndices() const;
	std::vector<Texture> getTextures() const;

private:
	unsigned int VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	void setupMesh();
};