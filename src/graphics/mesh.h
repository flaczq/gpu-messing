#pragma once

#include "../utils/config.h"
#include "../graphics/shader.h"

class Mesh {
public:
	// move
	Mesh(Mesh&& other) noexcept;
	// move assignment operator
	Mesh& operator=(Mesh&& other) noexcept;
	// block copy
	Mesh(const Mesh&) = delete;
	// block copy assignment operator
	Mesh& operator=(const Mesh&) = delete;
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	~Mesh();

	void draw(Shader &shader);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

private:
	unsigned int VAO, VBO, EBO;

	void setupMesh();
};