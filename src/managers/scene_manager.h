#pragma once

#include "../utils/config.h"
#include "../scenes/scene.h"

class SceneManager {
public:
	SceneManager(Camera* camera);

	bool init();
	void toggleScene();
	void update(float dt);
	void render();

	std::unique_ptr<Scene> currentScene;

private:
	Camera* camera = nullptr;
};