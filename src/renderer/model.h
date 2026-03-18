#pragma once

#include "../utils/commongl.h"
#include "../renderer/shader.h"
#include "../renderer/mesh.h"

class Model {
public:
	Model(const std::string& modelPath);
	~Model();

	void draw(Shader& shader);
	std::vector<Mesh>& getMeshes();

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> texturesLoaded;

	void loadModel(const std::string& modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	unsigned int loadTextureFromMemory(const aiTexture*, std::string filename);
	unsigned int loadTextureFromFile(std::string filename);
	void loadTexture(unsigned char* data, unsigned int& textureID, int data_w, int data_h, int data_ch);
};