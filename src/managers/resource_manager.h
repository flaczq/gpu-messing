#pragma once

#include "../graphics/graphics_types.hpp"
#include "../graphics/model.h"
#include "../graphics/shader.h"
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

	void loadModel(const std::string& name, const std::string& path);
	void loadShader(const std::string& name, const char* vertPath, const char* fragPath);
	// used to manually load texture from file
	void loadTexture(const std::string& name, const char* path, const std::string& typeName);

	Model* getModel(const std::string& name);
	Shader* getShader(const std::string& name);
	// used by Assimp to load textures from Model
	Texture* getTexture(const std::string& path, const std::string& type, const aiScene* scene);

	void clear();

private:
	// hidden constructor
	ResourceManager();

	std::unordered_map<std::string, std::shared_ptr<Model>> m_models;
	std::unordered_map<std::string, std::shared_ptr<Shader>> m_shaders;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;

	unsigned int loadTextureFromMemory(const aiTexture* embedded, std::string filename);
	unsigned int loadTextureFromFile(std::string filename);
	void uploadToGPU(unsigned char* data, unsigned int& textureID, int width, int height, int channels, bool isBGR);
};