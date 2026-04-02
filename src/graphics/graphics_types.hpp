#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include <string>
#include <utility>

constexpr auto MAX_BONE_INFLUENCE = 4;

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
	Texture(Texture&& other) noexcept : id(other.id), type(std::move(other.type)) {
		other.id = 0;
	}
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	Texture() = default;
	~Texture() {
		if (id != 0) {
			glDeleteTextures(1, &id);
		}
	}

	unsigned int id;
	std::string type;
	std::string path;
};