#include "scene_manager.h"
#include "../scenes/soldier_scene.h"

SceneManager::SceneManager(Camera* camera) {
	this->camera = camera;
}

bool SceneManager::init() {
	std::unique_ptr<Scene> defaultScene = std::make_unique<SoldierScene>(camera);
	currentScene = std::move(defaultScene);
	currentScene->init();

	return 1;
}

void SceneManager::toggleScene() {
	if (currentScene) {
		currentScene->end();
	}

	std::unique_ptr<Scene> nextScene;
	std::string sceneIDStr;
	if (currentScene->sceneID == SceneID::SOLDIER) {
		// FIXME TODO
		nextScene = std::make_unique<SoldierScene>(camera);
		sceneIDStr = "LIGHTS_ROOM";
	} else if (currentScene->sceneID == SceneID::LIGHTS_ROOM) {
		// FIXME TODO
		nextScene = std::make_unique<SoldierScene>(camera);
		sceneIDStr = "REAL_GAME";
	} else {
		nextScene = std::make_unique<SoldierScene>(camera);
		sceneIDStr = "SOLDIER";
	}

	currentScene = std::move(nextScene);
	currentScene->init();
	std::cout << "* Changed CurrentScene to: " << sceneIDStr << std::endl << std::endl;
}

void SceneManager::update(float dt) {
	if (currentScene) {
		currentScene->update(dt);
	}
}

void SceneManager::render() {
	if (currentScene) {
		currentScene->render();
	}
}