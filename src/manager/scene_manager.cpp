#include "scene_manager.h"
#include "../scene/soldier_scene.h"

bool SceneManager::init(GLFWwindow* window) {
	std::unique_ptr<Scene> defaultScene = std::make_unique<SoldierScene>();
	currentScene = std::move(defaultScene);
	currentScene->init(window);

	return 1;
}

void SceneManager::toggleScene(GLFWwindow* window) {
	if (currentScene) {
		currentScene->end();
	}

	std::unique_ptr<Scene> nextScene;
	std::string sceneIDStr;
	if (currentScene->getSceneID() == SceneID::SOLDIER) {
		// FIXME TODO
		nextScene = std::make_unique<SoldierScene>();
		sceneIDStr = "LIGHTS_ROOM";
	} else if (currentScene->getSceneID() == SceneID::LIGHTS_ROOM) {
		// FIXME TODO
		nextScene = std::make_unique<SoldierScene>();
		sceneIDStr = "REAL_GAME";
	} else {
		nextScene = std::make_unique<SoldierScene>();
		sceneIDStr = "SOLDIER";
	}

	currentScene = std::move(nextScene);
	currentScene->init(window);
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

Scene& SceneManager::getCurrentScene() {
	return *currentScene;
}