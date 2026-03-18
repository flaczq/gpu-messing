#include "model.h"
#include "../../libs/stb_image.h"

Model::Model(const std::string& modelPath) {
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

// cannot copy, must be a reference
std::vector<Mesh>& Model::getMeshes() {
	return meshes;
}

void Model::loadModel(const std::string& modelPath) {
	Assimp::Importer importer{};
	// convert all primitives into triangles | flip texture Y axis | generate smooth normals (if missing) | calculate tangents
	const aiScene* scene = importer.ReadFile(modelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
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
		Mesh newMesh = processMesh(mesh, scene);
		meshes.push_back(std::move(newMesh));
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
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular maps
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
	std::vector<Texture> textures;

	for (size_t i{}; i < mat->GetTextureCount(type); i++) {
		aiString str;
		// load texture into str
		mat->GetTexture(type, i, &str);
		const char* pathOrMem = str.C_Str();
		bool skip = false;
		for (size_t j{}; j < texturesLoaded.size(); j++) {
			// just copy if texture was already loaded (optimization)
			if (std::strcmp(texturesLoaded[j].path.data(), pathOrMem) == 0) {
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			std::string filename = directory + '/' + std::string(pathOrMem);
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(pathOrMem);
			// check if texture is embedded (.glb file) or outside (.png file)
			if (embeddedTexture) {
				texture.id = loadTextureFromMemory(embeddedTexture, filename);
			} else {
				texture.id = loadTextureFromFile(filename);
			}

			texture.type = typeName;
			texture.path = pathOrMem;
			textures.push_back(texture);
			texturesLoaded.push_back(texture);
		}
	}

	return textures;
}

unsigned int Model::loadTextureFromMemory(const aiTexture* textureMem, std::string filename) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(true);

	int data_w, data_h, data_ch;
	// compressed texture format: ARGB8888
	unsigned int bufferSize = textureMem->mHeight == 0 ? textureMem->mWidth : textureMem->mWidth * textureMem->mHeight * 4;
	unsigned char* data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(textureMem->pcData), bufferSize, &data_w, &data_h, &data_ch, 0);

	if (data) {
		loadTexture(data, textureID, data_w, data_h, data_ch);
	} else {
		std::cout << "ERROR::MODEL::ASSIMP_LOAD_FROM_MEMORY_FAILED: " << filename << std::endl;
	}

	return textureID;
}

unsigned int Model::loadTextureFromFile(std::string filename) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	//stbi_set_flip_vertically_on_load(true);

	int data_w, data_h, data_ch;
	unsigned char* data = stbi_load(filename.c_str(), &data_w, &data_h, &data_ch, 0);
	if (data) {
		loadTexture(data, textureID, data_w, data_h, data_ch);
	} else {
		std::cout << "ERROR::MODEL::ASSIMP_LOAD_FROM_FILE_FAILED: " << filename << std::endl;
	}

	return textureID;
}

void Model::loadTexture(unsigned char* data, unsigned int& textureID, int data_w, int data_h, int data_ch) {
	// data_ch == 1
	GLenum format = GL_RED;
	if (data_ch == 2) {
		format = GL_RG;
	} else if (data_ch == 3) {
		format = GL_RGB;
	} else if (data_ch == 4) {
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

	stbi_image_free(data);
}