#pragma once

#include "graphics_types.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Shader;
class Mesh;

class Model {
public:
	Model(const std::string& path);
	~Model();

	void draw(Shader& shader);

private:
	std::vector<std::unique_ptr<Mesh>> m_meshes;
	std::string m_directory;

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) const;
};