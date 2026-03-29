#include "../configs/log_config.hpp"
#include "../cores/camera.h"
#include "../scenes/scene.h"
#include "../scenes/soldier_scene.h"
#include "scene_manager.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>

SceneManager::SceneManager(Camera* camera)
	: m_camera(camera)
{
}

SceneManager::~SceneManager() = default;

bool SceneManager::init() {
	// default scene
	m_currentScene = std::make_unique<SoldierScene>(m_camera);
	m_currentScene->init();

	return true;
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
		sceneIDStr = "FPS_GAME";
	} else {
		nextScene = std::make_unique<SoldierScene>(m_camera);
		sceneIDStr = "SOLDIER";
	}

	m_currentScene = std::move(nextScene);
	m_currentScene->init();
	LOG_D("Changed CurrentScene to: " << sceneIDStr);
}

void SceneManager::update(float dt) const {
	if (m_currentScene) {
		m_currentScene->update(dt);
	}
}

void SceneManager::renderFrame() const {
	if (m_currentScene) {
		m_currentScene->render();
	}
}