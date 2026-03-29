#include "../scenes/soldier_scene.h"
#include "../scenes/scene.h"
#include "scene_manager.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>

class Camera;

SceneManager::SceneManager(Camera* camera) {
	m_camera = camera;
}

SceneManager::~SceneManager() = default;

bool SceneManager::init() {
	// default scene
	m_currentScene = std::make_unique<SoldierScene>(m_camera);
	m_currentScene->init();

	return 1;
}

void SceneManager::toggleScene() {
	if (m_currentScene) {
		m_currentScene->end();
	}

	std::unique_ptr<Scene> nextScene;
	std::string sceneIDStr;
	if (m_currentScene->getSceneID() == SceneID::SOLDIER) {
		// FIXME TODO
		nextScene = std::make_unique<SoldierScene>(m_camera);
		sceneIDStr = "LIGHTS_ROOM";
	} else if (m_currentScene->getSceneID() == SceneID::LIGHTS_ROOM) {
		// FIXME TODO
		nextScene = std::make_unique<SoldierScene>(m_camera);
		sceneIDStr = "REAL_GAME";
	} else {
		nextScene = std::make_unique<SoldierScene>(m_camera);
		sceneIDStr = "SOLDIER";
	}

	m_currentScene = std::move(nextScene);
	m_currentScene->init();
	std::cout << "* Changed CurrentScene to: " << sceneIDStr << std::endl;
}

void SceneManager::update(float dt) const {
	if (m_currentScene) {
		m_currentScene->update(dt);
	}
}

void SceneManager::render() const {
	if (m_currentScene) {
		m_currentScene->render();
	}
}