#include "model.h"
#include "mesh.h"
#include "../../libs/stb_image.h"
#include "graphics_types.hpp"
#include "../configs/gl_config.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <utility>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(const std::string& modelPath) {
	loadModel(modelPath);
}

Model::~Model() {
	for (auto& it : m_texturesLoaded) {
		glDeleteTextures(1, &it.second.id);
	}
}

void Model::draw(Shader& shader) {
	for (size_t i{}; i < m_meshes.size(); i++) {
		m_meshes[i].draw(shader);
	}
}

void Model::loadModel(const std::string& modelPath) {
	Assimp::Importer importer{};
	// convert all primitives into triangles | flip texture Y axis | generate smooth normals (if missing) | calculate tangents
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	if (!scene || !scene->mRootNode || (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE)) {
		std::cout << "ERROR::MODEL::ASSIMP_IMPORT_FAILED: " << modelPath << std::endl;
		return;
	}

	m_directory = modelPath.substr(0, modelPath.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process all mesh nodes (if exist)
	for (size_t i{}; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = processMesh(mesh, scene);
		m_meshes.push_back(std::move(newMesh));
	}

	// recursive: process all nodes' children
	for (size_t i{}; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// vertices
	for (size_t i{}; i < mesh->mNumVertices; i++) {
		Vertex vertex{};
		// Position
		glm::vec3 vector{};
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// Normal
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// TexCoords (if exist)
		if (mesh->mTextureCoords[0] && mesh->GetNumUVChannels() > 0) {
			auto& texCoords = mesh->mTextureCoords[0];
			glm::vec2 vec{};
			vec.x = texCoords[i].x;
			vec.y = texCoords[i].y;
			vertex.TexCoords = vec;
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

	// textures (if exist)
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
	std::vector<Texture> textures;

	for (size_t i{}; i < mat->GetTextureCount(type); i++) {
		aiString str;
		// load texture into str
		mat->GetTexture(type, i, &str);
		const char* pathOrMem = str.C_Str();
		
		auto it = m_texturesLoaded.find(pathOrMem);
		// just load if texture was already cached
		if (it != m_texturesLoaded.end()) {
			textures.push_back(it->second);
			continue;
		}

		// load if not already cached
		Texture texture;
		std::string filename = m_directory + '/' + std::string(pathOrMem);
		const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(pathOrMem);
		// check if texture is embedded (.glb file) or outside (.png file)
		if (embeddedTexture) {
			texture.id = loadTextureFromMemory(embeddedTexture, filename);
		} else {
			texture.id = loadTextureFromFile(filename);
		}

		texture.type = typeName;
		texture.path = pathOrMem;
		textures.push_back(texture);
		m_texturesLoaded.emplace(pathOrMem, texture);
	}

	return textures;
}

unsigned int Model::loadTextureFromMemory(const aiTexture* textureMem, std::string filename) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data;
	bool isStbiData = false;
	bool isBGR = false;
	if (textureMem->mHeight == 0) {
		// compressed texture format (PNG/JPG), force 4 channels (RGBA)
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(textureMem->pcData), textureMem->mWidth, &width, &height, &channels, 4);
		// just to be sure
		channels = 4;
		isStbiData = true;
		isBGR = false;
	} else {
		// raw data (ARGB/BGRA)
		width = textureMem->mWidth;
		height = textureMem->mHeight;
		// 4 bytes
		unsigned int bufforSize = width * height * 4;
		// allocate buffor
		data = (unsigned char*)malloc(bufforSize);
		if (data) {
			memcpy(data, textureMem->pcData, bufforSize);
		}
		channels = 4;
		isStbiData = false;
		isBGR = true;
	}
	
	if (data) {
		loadTexture(data, textureID, width, height, channels, isBGR);

		if (isStbiData) {
			stbi_image_free(data);
		} else {
			free(data);
		}
	} else {
		std::cout << "ERROR::MODEL::ASSIMP_LOAD_FROM_MEMORY_FAILED: " << filename << std::endl;
	}

	return textureID;
}

unsigned int Model::loadTextureFromFile(std::string filename) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (data) {
		// always RGB/RGBA so isBGR = false
		loadTexture(data, textureID, width, height, channels, false);
		stbi_image_free(data);
	} else {
		std::cout << "ERROR::MODEL::ASSIMP_LOAD_FROM_FILE_FAILED: " << filename << std::endl;
	}

	return textureID;
}

void Model::loadTexture(unsigned char* data, unsigned int& textureID, int width, int height, int channels, bool isBGR) {
	GLenum gpuFormat = GL_RGBA;
	GLenum dataFormat = GL_RGBA;
	
	if (channels == 1) {
		gpuFormat = dataFormat = GL_RED;
	} else if (channels == 3) {
		gpuFormat = dataFormat = isBGR ? GL_BGR : GL_RGB;
	} else if (channels == 4) {
		gpuFormat = GL_RGBA;
		dataFormat = isBGR ? GL_BGR : GL_RGBA;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, gpuFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	// filter params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}