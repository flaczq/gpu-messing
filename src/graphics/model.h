#pragma once

#include "graphics_types.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <map>
#include <string>
#include <vector>

class Shader;
class Mesh;

class Model {
public:
	Model(const std::string& path);

	void draw(Shader& shader);

private:
	std::vector<Mesh> m_meshes;
	std::string m_directory;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) const;
};