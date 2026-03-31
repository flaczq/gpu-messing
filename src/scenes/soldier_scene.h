#pragma once

#include "../configs/gl_config.hpp"
#include "../configs/math_config.hpp"
#include "../graphics/graphics_types.hpp"
#include "scene.h"

class Camera;
class Model;
class Mesh;

class SoldierScene : public Scene {
public:
	SoldierScene(Camera* camera);

	void init() override;
	void fixedUpdate(float fixedt) override;
	void renderFrame(float alpha) override;
	void end() override;

private:
	SceneID m_sceneID = SceneID::SOLDIER;
	std::vector<std::unique_ptr<GameObject>> m_gameObjects;

	Camera* m_camera = nullptr;

	std::unique_ptr<Model> m_soldier;
	std::unique_ptr<Mesh> m_lightMarker;
	std::vector<glm::mat4> m_soldierModels;

	glm::mat4 m_lightModel = glm::mat4(1.0f);
	glm::mat4 m_projection = glm::mat4(1.0f);
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat3 m_normalMatrix = glm::mat3(1.0f);
	glm::vec3 m_lightDir = glm::normalize(glm::vec3(0.5f, -1.0f, -0.5f));
	glm::vec3 m_soldierPos = glm::vec3(3.0f, 0.0f, 3.0f);
	glm::vec3 m_soldierPreviousPos = glm::vec3(3.0f, 0.0f, 3.0f);
	float m_rotation = 0.0f;
	float m_rotationSpeed = 50.0f;
	size_t soldiersCount = 10;

	std::vector<Vertex> calculateLightVertices();
	std::vector<unsigned int> calculateLightIndices();
};