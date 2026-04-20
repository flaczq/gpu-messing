#pragma once

#include "../graphics/graphics_types.hpp"
#include "../graphics/material.h"
#include "../graphics/model.h"
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
	// Meyer's Singleton
	static ResourceManager& getInstance();
	ResourceManager(const ResourceManager&) = delete;
	void operator=(const ResourceManager&) = delete;

	// add model with meshes created via MeshGenerator
	void addModel(const std::string& name, std::shared_ptr<Model> model);
	void loadModel(const std::string& name, const std::string& path);
	void loadMaterial(const std::string& name, const char* vertPath, const char* fragPath);
	// manually load texture from file
	void loadTexture(const std::string& name, const char* path, const std::string& typeName);

	std::shared_ptr<Model> getModel(const std::string& name);
	std::shared_ptr<Material> getMaterial(const std::string& name);
	// used by Assimp to load textures from Model
	std::shared_ptr<Texture> getTexture(const std::string& path, const std::string& type, const aiScene* scene);

	void clear();

private:
	// hidden constructor
	ResourceManager();

	std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
	std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

	unsigned int loadTextureFromMemory(const aiTexture* textureMem, const std::string& path);
	unsigned int loadTextureFromFile(const std::string& path);
	void uploadToGPU(unsigned char* data, unsigned int& textureID, int width, int height, int channels, bool isBGR);
};