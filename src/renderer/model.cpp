#include "model.h"
#include "../../libs/stb_image.h"

Model::Model(const std::string &modelPath) {
	loadModel(modelPath);
}

Model::~Model() {
	for (auto& tex : texturesLoaded) {
		glDeleteTextures(1, &tex.id);
	}
}

void Model::draw(Shader& shader) {
	for (size_t i{}; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::loadModel(const std::string &modelPath) {
	Assimp::Importer importer{};
	// convert all primitives into triangles | flip texture Y axis
	const aiScene *scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!scene || !scene->mRootNode || (scene->mFlags && AI_SCENE_FLAGS_INCOMPLETE)) {
		std::cout << "ERROR::MODEL::ASSIMP_IMPORT_FAILED: " << modelPath << std::endl;
		return;
	}

	directory = modelPath.substr(0, modelPath.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	// process all mesh nodes (if exist)
	for (size_t i{}; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// recursive: process all nodes' children
	for (size_t i{}; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// vertices
	for (size_t i{}; i < mesh->mNumVertices; i++) {
		Vertex vertex{};
		// Position
		glm::vec3 vector{};
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		// Normal
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// TexCoords (if exist)
		if (mesh->mTextureCoords[0] && mesh->GetNumUVChannels() > 0) {
			auto& texCoords = mesh->mTextureCoords[0];
			glm::vec2 vec{};
			vec.x = texCoords[i].x;
			vec.y = texCoords[i].y;
			vertex.TexCoords = vec;
		}

		// Tangent TODO

		// Bitangent TODO

		vertices.push_back(vertex);
	}

	// indices
	for (size_t i{}; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j{}; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// textures (if exist)
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse maps
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;

	for (size_t i{}; i < mat->GetTextureCount(type); i++) {
		aiString str;
		// get texture into str
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (size_t j{}; j < texturesLoaded.size(); j++) {
			// just copy if texture was already loaded
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.id = loadTextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			// optimization
			texturesLoaded.push_back(texture);
		}
	}

	return textures;
}

unsigned int Model::loadTextureFromFile(const char* texturePath, std::string directory) {
	std::string filename = directory + '/' + std::string(texturePath);
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(true);

	int data_w, data_h, data_ch;
	unsigned char* data = stbi_load(filename.c_str(), &data_w, &data_h, &data_ch, 0);
	if (data) {
		// data_ch == 1
		GLenum format = GL_RED;
		if (data_ch == 3) {
			format = GL_RGB;
		}
		else if (data_ch == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, data_w, data_h, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// filter params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		std::cout << "ERROR::MODEL::ASSIMP_IMPORT_FAILED: " << filename << std::endl;
	}

	stbi_image_free(data);

	return textureID;
}