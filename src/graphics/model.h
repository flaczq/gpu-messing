#pragma once

#include "graphics_types.hpp"
#include <vector>
#include <string>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Shader;
class Mesh;

class Model {
public:
	Model(const std::string& modelPath);
	~Model();

	void draw(Shader& shader);

private:
	std::vector<Mesh> m_meshes;
	std::string m_directory;
	std::map<std::string, Texture> m_texturesLoaded;

	void loadModel(const std::string& modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	unsigned int loadTextureFromMemory(const aiTexture*, std::string filename);
	unsigned int loadTextureFromFile(std::string filename);
	void loadTexture(unsigned char* data, unsigned int& textureID, int width, int height, int channels, bool isBGR);
};