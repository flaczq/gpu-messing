#pragma once

#include "../utils/commongl.h"
#include "../scene/scene.h"

class SceneManager {
public:
	bool init(GLFWwindow* window);
	void toggleScene(GLFWwindow* window);
	void update(float dt);
	void render();

	Scene& getCurrentScene();

private:
	std::unique_ptr<Scene> currentScene;
};