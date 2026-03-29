#pragma once

#include <string>
#include "../configs/math_config.hpp"

constexpr auto MAX_BONE_INFLUENCE = 4;

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	int BoneIDs[MAX_BONE_INFLUENCE];
	float Weights[MAX_BONE_INFLUENCE];
};