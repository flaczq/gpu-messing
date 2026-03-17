#pragma once

#include "../utils/commongl.h"
#include "../renderer/shader.h"
#include "../renderer/mesh.h"

class Model {
public:
	Model(const std::string &modelPath);
	~Model();

	void draw(Shader& shader);

private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> texturesLoaded;

	void loadModel(const std::string &modelPath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	unsigned int loadTextureFromFile(const char* texturePath, std::string directory);
};