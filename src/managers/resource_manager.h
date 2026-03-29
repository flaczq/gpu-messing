#pragma once

#include "../graphics/graphics_types.hpp"
#include "../graphics/shader.h"
#include <assimp/scene.h>
#include <memory>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
	static void loadShader(const std::string& name, const char* vertPath, const char* fragPath);
	// used to manually load some textures
	static void loadTexture(const std::string& name, const char* path, const std::string& typeName);

	static Shader* getShader(const std::string& name);
	// used by Assimp to load textures from Model
	static Texture* getTexture(const std::string& path, const std::string& type, const aiScene* scene);

	static void clear();

private:
	// singleton
	ResourceManager() = default;

	// no need to define them in the .cpp
	inline static std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
	inline static std::unordered_map<std::string, Texture> m_textures;

	static unsigned int loadTextureFromMemory(const aiTexture* embedded, std::string filename);
	static unsigned int loadTextureFromFile(std::string filename);
	static void uploadToGPU(unsigned char* data, unsigned int& textureID, int width, int height, int channels, bool isBGR);
};