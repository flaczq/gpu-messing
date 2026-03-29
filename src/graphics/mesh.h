#pragma once

#include "graphics_types.hpp"
#include <vector>

class Shader;

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
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
	~Mesh();

	void draw(Shader &shader);

private:
	unsigned int m_VAO{}, m_VBO{}, m_EBO{};

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;
	std::vector<Texture*> m_textures;

	void setupMesh();
};