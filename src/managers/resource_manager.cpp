#include "../../libs/stb_image.h"
#include "../configs/log_config.hpp"
#include "../graphics/graphics_types.hpp"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include "../graphics/shader.h"
#include "resource_manager.h"
#include <assimp/scene.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

ResourceManager& ResourceManager::getInstance() {
	static ResourceManager instance;
	return instance;
}

ResourceManager::ResourceManager() = default;

void ResourceManager::addModel(std::shared_ptr<Model> model) {
	const std::string& name = model->getName();
	if (m_models.find(name) != m_models.end()) {
		LOG("Already added Model: " << name);
		return;
	}

	m_models[name] = model;
	LOG("Adding Model: " << name << " with mesh from MeshGenerator");
}

void ResourceManager::loadModel(const std::string& name, const std::string& path) {
	if (m_models.find(name) != m_models.end()) {
		LOG("Already loaded Model: " << name);
		return;
	}

	m_models[name] = std::make_shared<Model>(name, path);
	LOG("Loading Model: " << name << " from path: " << path);
}

void ResourceManager::loadMaterial(const std::string& name, const char* vertPath, const char* fragPath) {
	if (m_materials.find(name) != m_materials.end()) {
		LOG("Already loaded Material: " << name);
		return;
	}

	auto shader = std::make_shared<Shader>(vertPath, fragPath);
	m_materials[name] = std::make_shared<Material>(name, shader);
	LOG("Loading Material: " << name << " from path: " << vertPath << " and " << fragPath);
}

void ResourceManager::loadTexture(const std::string& name, const char* path, const std::string& typeName) {
	if (m_textures.find(name) != m_textures.end()) {
		LOG("Already loaded Texture: " << name);
		return;
	}

	unsigned int textureID = loadTextureFromFile(path);
	auto texture = std::make_shared<Texture>();
	texture->id = textureID;
	texture->type = typeName;
	texture->path = path;

	m_textures[name] = std::move(texture);
	LOG("Loading Texture: " << name << " from path: " << path);
}

std::shared_ptr<Model> ResourceManager::getModel(const std::string& name) {
	auto it = m_models.find(name);
	if (it == m_models.end()) {
		LOG_E("RESOURCE_MANAGER::GET_MODEL_NULLPTR: " << name);
		return nullptr;
	}

	return it->second;
}

std::shared_ptr<Material> ResourceManager::getMaterial(const std::string& name) {
	auto it = m_materials.find(name);
	if (it == m_materials.end()) {
		LOG_E("RESOURCE_MANAGER::GET_MATERIAL_NULLPTR: " << name);
		return nullptr;
	}

	return it->second;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& name) {
	auto it = m_textures.find(name);
	if (it == m_textures.end()) {
		LOG_E("RESOURCE_MANAGER::GET_TEXTURE_NULLPTR: " << name);
		return nullptr;
	}

	return it->second;
}

std::shared_ptr<Texture> ResourceManager::getTexture(const std::string& path, const std::string& type, const aiScene* scene) {
	// filename for Assimp
	size_t lastSlash = path.find_last_of('/');
	std::string name = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);
	
	auto it = m_textures.find(name);
	// just load if texture is cached
	if (it != m_textures.end()) {
		LOG_D("Using cache for Texture: " << path);
		return it->second;
	}

	auto texture = std::make_shared<Texture>();
	texture->type = type;
	texture->path = path;

	// check if texture is embedded (.glb file)
	if (scene) {
		const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(name.c_str());
		// again check if texture is embedded or outside (.png file)
		if (embeddedTexture) {
			texture->id = loadTextureFromMemory(embeddedTexture, path);
		} else {
			texture->id = loadTextureFromFile(path);
		}
	} else {
		texture->id = loadTextureFromFile(path);
	}


	m_textures[name] = std::move(texture);
	return m_textures[name];
}

void ResourceManager::reloadShaders() {
	for (const auto& [name, material] : m_materials) {
		material->getShader()->reload();
	}
	LOG_D("Hot-loaded shaders...");
}

void ResourceManager::end() {
	m_models.clear();
	m_materials.clear();
	m_textures.clear();
}

unsigned int ResourceManager::loadTextureFromMemory(const aiTexture* textureMem, const std::string& path) {
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
		uploadToGPU(data, textureID, width, height, channels, isBGR);

		if (isStbiData) {
			stbi_image_free(data);
		} else {
			free(data);
		}
	} else {
		LOG_E("RESOURCE_MANAGER::ASSIMP_LOAD_TEXTURE_FROM_MEMORY_FAILED: " << path);
	}

	return textureID;
}

unsigned int ResourceManager::loadTextureFromFile(const std::string& path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (data) {
		// always RGB/RGBA so isBGR = false
		uploadToGPU(data, textureID, width, height, channels, false);
		stbi_image_free(data);
	} else {
		LOG_E("RESOURCE_MANAGER::ASSIMP_LOAD_TEXTURE_FROM_FILE_FAILED: " << path);
	}

	return textureID;
}

void ResourceManager::uploadToGPU(unsigned char* data, unsigned int& textureID, int width, int height, int channels, bool isBGR) {
	GLenum gpuFormat = GL_RGBA;
	GLenum dataFormat = GL_RGBA;

	if (channels == 1) {
		gpuFormat = dataFormat = GL_RED;
	} else if (channels == 3) {
		gpuFormat = dataFormat = (isBGR) ? GL_BGR : GL_RGB;
	} else if (channels == 4) {
		gpuFormat = GL_RGBA;
		dataFormat = (isBGR) ? GL_BGR : GL_RGBA;
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