#pragma once

#include "../configs/math_config.hpp"
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
	Model(const std::string& name, const std::string& path);
	Model(const std::string& name, std::unique_ptr<Mesh> mesh, glm::vec3 AABBMin = glm::vec3(0.0f), glm::vec3 AABBMax = glm::vec3(0.0f));
	~Model();

	const std::string& getName() const { return m_name; }
	glm::vec3 getAABBMin() const { return m_AABBMin; }
	glm::vec3 getAABBMax() const { return m_AABBMax; }

	void draw(Shader& shader);

private:
	std::string m_name{};
	std::string m_directory{};
	std::vector<std::unique_ptr<Mesh>> m_meshes;
	glm::vec3 m_AABBMin{};
	glm::vec3 m_AABBMax{};

	void loadModel(const std::string& path);
	void processNode(aiNode* node, const aiScene* scene);
	std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<std::shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) const;
};