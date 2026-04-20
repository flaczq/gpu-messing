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
		v2.TexCoords = { uvTiling, 0.0f     };
		v3.TexCoords = { 0.0f,     0.0f     };

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

	Mesh createCube(float width, float height, float depth, float uvTiling) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<std::shared_ptr<Texture>> textures;

		float halfWidth  = width  / 2.0f;
		float halfHeight = height / 2.0f;
		float halfDepth  = depth  / 2.0f;

		Vertex d0{}, d1{}, d2{}, d3{}, //down
			   u0{}, u1{}, u2{}, u3{}, //up
			   b0{}, b1{}, b2{}, b3{}, //back
			   f0{}, f1{}, f2{}, f3{}, //front
			   l0{}, l1{}, l2{}, l3{}, //left
			   r0{}, r1{}, r2{}, r3{}; //right

		d0.Position = { -halfWidth, -halfHeight, -halfDepth };
		d1.Position = {  halfWidth, -halfHeight, -halfDepth };
		d2.Position = {  halfWidth, -halfHeight,  halfDepth };
		d3.Position = { -halfWidth, -halfHeight,  halfDepth };
		u0.Position = { -halfWidth,  halfHeight, -halfDepth };
		u1.Position = {  halfWidth,  halfHeight, -halfDepth };
		u2.Position = {  halfWidth,  halfHeight,  halfDepth };
		u3.Position = { -halfWidth,  halfHeight,  halfDepth };
		b0.Position = { -halfWidth, -halfHeight, -halfDepth };
		b1.Position = {  halfWidth, -halfHeight, -halfDepth };
		b2.Position = {  halfWidth,  halfHeight, -halfDepth };
		b3.Position = { -halfWidth,  halfHeight, -halfDepth };
		f0.Position = { -halfWidth, -halfHeight,  halfDepth };
		f1.Position = {  halfWidth, -halfHeight,  halfDepth };
		f2.Position = {  halfWidth,  halfHeight,  halfDepth };
		f3.Position = { -halfWidth,  halfHeight,  halfDepth };
		l0.Position = { -halfWidth, -halfHeight,  halfDepth };
		l1.Position = { -halfWidth, -halfHeight, -halfDepth };
		l2.Position = { -halfWidth,  halfHeight, -halfDepth };
		l3.Position = { -halfWidth,  halfHeight,  halfDepth };
		r0.Position = {  halfWidth, -halfHeight,  halfDepth };
		r1.Position = {  halfWidth, -halfHeight, -halfDepth };
		r2.Position = {  halfWidth,  halfHeight, -halfDepth };
		r3.Position = {  halfWidth,  halfHeight,  halfDepth };

		d0.TexCoords = u0.TexCoords = b0.TexCoords = f0.TexCoords = l0.TexCoords = r0.TexCoords = { 0.0f,             0.0f              };
		d1.TexCoords = u1.TexCoords = b1.TexCoords = f1.TexCoords = l1.TexCoords = r1.TexCoords = { width * uvTiling, 0.0f              };
		d2.TexCoords = u2.TexCoords = b2.TexCoords = f2.TexCoords = l2.TexCoords = r2.TexCoords = { width * uvTiling, height * uvTiling };
		d3.TexCoords = u3.TexCoords = b3.TexCoords = f3.TexCoords = l3.TexCoords = r3.TexCoords = { 0.0f,             height * uvTiling };

		d0.Normal = d1.Normal = d2.Normal = d3.Normal = {  0.0f, -1.0f,  0.0f };
		u0.Normal = u1.Normal = u2.Normal = u3.Normal = {  0.0f,  1.0f,  0.0f };
		b0.Normal = b1.Normal = b2.Normal = b3.Normal = {  0.0f,  0.0f, -1.0f };
		f0.Normal = f1.Normal = f2.Normal = f3.Normal = {  0.0f,  0.0f,  1.0f };
		l0.Normal = l1.Normal = l2.Normal = l3.Normal = { -1.0f,  0.0f,  0.0f };
		r0.Normal = r1.Normal = r2.Normal = r3.Normal = {  1.0f,  0.0f,  0.0f };

		vertices = {
			d0, d1, d2, d3,
			u0, u1, u2, u3,
			b0, b1, b2, b3,
			f0, f1, f2, f3,
			l0, l1, l2, l3,
			r0, r1, r2, r3
		};
		indices = {
			0,   1,  2,  2,  3,  0,
			4,   5,  6,  6,  7,  4,
			8,   9, 10, 10, 11,  8,
			12, 13, 14, 14, 15, 12,
			16, 17, 18, 18, 19, 16,
			20, 21, 22, 22, 23, 20
		};
		// empty
		textures = {};

		return Mesh(vertices, indices, textures);
	}
}