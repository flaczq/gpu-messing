#include "../configs/gl_config.hpp"
#include "../configs/log_config.hpp"
#include "../managers/resource_manager.h"
#include "graphics_types.hpp"
#include "mesh.h"
#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

Model::Model(const std::string& name, const std::string& path)
	: m_name(name)
{
	loadModel(path);
}

Model::Model(const std::string& name, std::unique_ptr<Mesh> mesh)
	: m_name(name)
{
	m_meshes.push_back(std::move(mesh));
}

Model::~Model() = default;

void Model::draw(Shader& shader) {
	for (size_t i{}; i < m_meshes.size(); i++) {
		m_meshes[i]->draw(shader);
	}
}

void Model::loadModel(const std::string& path) {
	Assimp::Importer importer{};
	// convert all primitives into triangles | flip texture Y axis | generate smooth normals (if missing) | calculate tangents
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	if (!scene || !scene->mRootNode || (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE)) {
		std::cerr << "ERROR::MODEL::ASSIMP_IMPORT_FAILED: " << path << std::endl;
		return;
	}

	m_directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process all mesh nodes (if exist)
	for (size_t i{}; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::unique_ptr<Mesh> newMesh = processMesh(mesh, scene);
		m_meshes.push_back(std::move(newMesh));
	}

	// recursive: process all nodes' children
	for (size_t i{}; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

std::unique_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<std::shared_ptr<Texture>> textures;
	bool hasVertexColor = false;

	// vertices
	for (size_t i{}; i < mesh->mNumVertices; i++) {
		Vertex vertex{};

		// Position
		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		// Color (if exist)
		if (mesh->HasVertexColors(0)) {
			vertex.Color.x = mesh->mColors[0][i].r;
			vertex.Color.y = mesh->mColors[0][i].g;
			vertex.Color.z = mesh->mColors[0][i].b;
			hasVertexColor = true;
		}

		// Normal
		vertex.Normal.x = mesh->mNormals[i].x;
		vertex.Normal.y = mesh->mNormals[i].y;
		vertex.Normal.z = mesh->mNormals[i].z;

		// TexCoords (if exist)
		if (mesh->HasTextureCoords(0) && mesh->GetNumUVChannels() > 0) {
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}

		// Tangent TODO

		// Bitangent TODO

		// boneIDs TODO

		// weights TODO

		vertices.push_back(vertex);
	}

	// indices
	for (size_t i{}; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j{}; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// material textures/colors (if exist)
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse maps
		auto diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular maps
		auto specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// no textures and no vertex colors
		if (textures.empty() && !hasVertexColor) {
			// diffuse color
			aiColor4D diff(1.0f, 1.0f, 1.0f, 1.0f);
			if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diff)) {
				auto diffuseColor = glm::vec3(diff.r, diff.g, diff.b);
				// TODO... save in Mesh and later setVec3 with this color into Shader
			}
		}
	}

	return std::make_unique<Mesh>(vertices, indices, textures);
}

std::vector<std::shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) const {
	std::vector<std::shared_ptr<Texture>> textures;

	for (size_t i{}; i < mat->GetTextureCount(type); i++) {
		aiString str;
		// load texture into str
		mat->GetTexture(type, static_cast<unsigned int>(i), &str);
		std::string pathOrMem = str.C_Str();
		// unique key for ResourceManager
		std::string fullPath = m_directory + '/' + pathOrMem;

		auto texture = ResourceManager::getInstance().getTexture(fullPath, typeName, scene);
		if (texture) {
			textures.push_back(texture);
		}
	}

	return textures;
}