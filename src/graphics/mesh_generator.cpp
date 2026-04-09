#include "graphics_types.hpp"
#include "mesh.h"
#include "mesh_generator.h"
#include <memory>
#include <vector>

namespace MeshGenerator {
	Mesh createPlane(float width, float depth, float uvTiling) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::shared_ptr<Texture>> textures;

		float halfWidth = width / 2.0f;
		float halfDepth = depth / 2.0f;

		Vertex v0{}, v1{}, v2{}, v3{};

		v0.Position = { -halfWidth, 0.0f, -halfDepth };
		v1.Position = {  halfWidth, 0.0f, -halfDepth };
		v2.Position = {  halfWidth, 0.0f,  halfDepth };
		v3.Position = { -halfWidth, 0.0f,  halfDepth };

		v0.TexCoords = { 0.0f,     uvTiling };
		v1.TexCoords = { uvTiling, uvTiling };
		v2.TexCoords = { uvTiling, 0.0f };
		v3.TexCoords = { 0.0f,     0.0f };

		// up
		v0.Normal = v1.Normal = v2.Normal = v3.Normal = { 0.0f, 1.0f, 0.0f };
		// parallel to X axis
		//v0.Tangent = v1.Tangent = v2.Tangent = v3.Tangent = { 1.0f, 0.0f, 0.0f };
		//v0.Bitangent = v1.Bitangent = v2.Bitangent = v3.Bitangent = { 0.0f, 0.0f, 1.0f };
		
		// reset bones
		/*for (size_t i{}; i < 4; i++) {
			v0.BoneIDs[i] = v1.BoneIDs[i] = v2.BoneIDs[i] = v3.BoneIDs[i] = -1;
			v0.Weights[i] = v1.Weights[i] = v2.Weights[i] = v3.Weights[i] = 0.0f;
		}*/

		vertices = { v0, v1, v2, v3 };
		indices = { 0, 1, 2, 2, 3, 0 };
		// empty
		textures = {};

		return Mesh(vertices, indices, textures);
	}

	Mesh createCube(float width, float depth, float height) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::shared_ptr<Texture>> textures;
		//TODO
		return Mesh(vertices, indices, textures);
	}
}