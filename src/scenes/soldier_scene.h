#pragma once

#include "../utils/config.h"
#include "../core/camera.h"
#include "../graphics/shader.h"
#include "../graphics/model.h"
#include "../graphics/mesh.h"
#include "scene.h"

class SoldierScene : public Scene {
public:
	SoldierScene(Camera* camera);

	void init() override;
	void update(float dt) override;
	void render() override;
	void end() override;

	SceneID sceneID = SceneID::SOLDIER;

private:
	Camera* camera = nullptr;

	std::unique_ptr<Model> soldier = nullptr;
	std::unique_ptr<Shader> soldierShader = nullptr;
	std::unique_ptr<Shader> lightShader = nullptr;
	std::unique_ptr<Mesh> lightMarker = nullptr;

	glm::mat4 soldierModel = glm::mat4(1.0f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat3 normalMatrix = glm::mat3(1.0f);
	glm::vec3 soldierPos = glm::vec3(3.0f, 0.0f, 3.0f);
	glm::vec3 lightDir = glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f));
	float rotation = 0.0f;
	float rotationSpeed = 50.0f;

	std::vector<Vertex> calculateLightVertices();
	std::vector<unsigned int> calculateLightIndices();
};