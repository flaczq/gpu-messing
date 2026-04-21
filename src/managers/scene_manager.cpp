#include "../configs/log_config.hpp"
#include "../game/camera.h"
#include "../graphics/renderer.h"
#include "../scenes/scene.h"
#include "../scenes/soldier_scene.h"
#include "scene_manager.h"
#include <iostream>
#include <memory>
#include <string>
#include <utility>

SceneManager& SceneManager::getInstance() {
	static SceneManager instance;
	return instance;
}

SceneManager::SceneManager() = default;

bool SceneManager::init(Camera* camera) {
	m_camera = camera;

	// default scene
	m_currentScene = std::make_unique<SoldierScene>(m_camera);
	m_currentScene->init();

	return true;
}

void SceneManager::toggleScene() {
	std::unique_ptr<Scene> nextScene;
	std::string sceneIDStr;

	if (m_currentScene) {
		m_currentScene->end();

		if (m_currentScene->getID() == SceneID::SOLDIER) {
			// FIXME TODO
			nextScene = std::make_unique<SoldierScene>(m_camera);
			sceneIDStr = "LIGHTS_ROOM";
		} else if (m_currentScene->getID() == SceneID::LIGHTS_ROOM) {
			// FIXME TODO
			nextScene = std::make_unique<SoldierScene>(m_camera);
			sceneIDStr = "FPS_GAME";
		} else {
			nextScene = std::make_unique<SoldierScene>(m_camera);
			sceneIDStr = "SOLDIER";
		}
	} else {
		nextScene = std::make_unique<SoldierScene>(m_camera);
		sceneIDStr = "SOLDIER";
	}

	m_currentScene = std::move(nextScene);
	m_currentScene->init();
	LOG_D("Changed CurrentScene to: " << sceneIDStr);
}

void SceneManager::saveState() const {
	if (m_currentScene) {
		m_currentScene->saveState();
	}
}

void SceneManager::fixedUpdate(float fixedt) const {
	if (m_currentScene) {
		m_currentScene->fixedUpdate(fixedt);
	}
}

void SceneManager::update(float alpha) const {
	if (m_currentScene) {
		m_currentScene->update(alpha);
	}
}

void SceneManager::end() const {
	if (m_currentScene) {
		m_currentScene->end();
	}
}