#pragma once

#include "../utils/commongl.h"
#include "scene.h"
#include "../renderer/shader.h"
#include "../renderer/model.h"
#include "../core/camera.h"

class SoldierScene : public Scene {
public:
	SoldierScene();
	~SoldierScene();

	void init(GLFWwindow* window) override;
	void update(GLFWwindow* window, float dt) override;
	void render() override;
	void end() override;

	SceneID getSceneID();
	Camera& getCamera();

private:
	SceneID sceneID = SceneID::SOLDIER;

	std::unique_ptr<Shader> modelShader;
	std::unique_ptr<Model> soldierModel;
	Camera camera{ nullptr };

	glm::mat4 model;
	glm::mat4 projection;
	glm::mat4 view;
};